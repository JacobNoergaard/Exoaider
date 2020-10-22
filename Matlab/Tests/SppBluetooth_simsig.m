clear all; clc;

disp('Connecting to devices..');
motor_controller = [] %SppBluetooth('F4418CA4AE30@Exo-Aider', 'demo_motor_controller', 60 * 1000);

left_sensor_band = SppBluetooth('0877A9E350CC@Exo-Aider', 'demo_left_sensor_band', 60 * 1000);
right_sensor_band = [] %SppBluetooth('74E80B12CFA4@Exo-Aider', 'demo_right_sensor_band', 60 * 1000);
devices = [motor_controller, left_sensor_band, right_sensor_band];

%%
disp('Starting simple tests..');
for device = devices
    is_spp = isa(device, 'SppBluetooth');
    if is_spp
        name = [device.name, ' (' device.description ')'];
    else
        name = 'SppManager';
    end

    device.sample_frequency = 1000;
    assert(device.sample_frequency == 1000);
    device.sample_frequency = 500;
    assert(device.sample_frequency == 500);

    device.send_signals_ratio = 10;
    assert(device.send_signals_ratio == 10);
    device.send_signals_ratio = 30;
    assert(device.send_signals_ratio == 30);

    device.sample_frequency = 1000;
    device.send_signals_ratio = 20;
    device.send_signals = true;
    assert(device.send_signals == true);
    device.flush;
    pause(1);
    signals_got = min(device.signal_n);
    assert(45 <= signals_got);
    device.send_signals = false;
    assert(device.send_signals == false);

    disp([' * ', name, ' - pass!']);
end

disp('All simple tests passed!');

%% Make a simulation

disp('Testing real-time simulation performance..');
time_to_run = 20;
sample_frequency = 1000;
send_signals_ratio = 40;
reference_frequency = 4/time_to_run;

% Initial values
n = 0;
t_last = 0;
u_integrator = 0;

% Initialize devices
motor_controller.send('u', [0, 0]);
for device = devices
    disp(['* Configuring "', device.name, '"']);
    device.sample_frequency = sample_frequency;
    device.send_signals_ratio = send_signals_ratio;
    send_frequency = device.send_frequency;
    device.send_signals = true;
end
for device = devices
   device.flush; 
end
disp(['Sampling frequency = ', num2str(sample_frequency), ' Hz']);
disp(['Sending frequency = ' , num2str(send_frequency), ' Hz']);

disp('Simulating...');
sim_start_time = tic;
while toc(sim_start_time) < time_to_run
    n = n + 1;
    
    % Take care of sheduling
    t_now = toc(sim_start_time);
    t_next = t_last + 1 / send_frequency;
    t_wait = t_next - t_now;
    if 0 < t_wait
        pause(t_wait);
        t_last = t_next;
    else
        t_last = toc(sim_start_time); 
        samples_behind = ceil(-t_wait*send_frequency);
        disp(['t = ', num2str(t_now, '%.1f'), 's - Can''t keep up! ', num2str(-t_wait* 1e3, '%.1f') , ' ms behind - ', num2str(samples_behind), ' samples.']);
    end

    % Calculate reference:
    y_n = motor_controller.get_signals('y', 0);
    t_n = t_now; %motor_controller.get_signals('t', -1);
    r_local = calculate_reference(t_n, reference_frequency);

    e = r_local - y_n;
    u_integrator = u_integrator + e / send_frequency;
    u_input = e * 1 + u_integrator * 10;
    u_input = e * 0.5 + u_integrator * 5;
    
    % Simulate the calculation of each sensorband
    for j = 1:2
        t_local = tic; 
        some_counter = 0;
        while  toc(t_local) < 5e-3
            some_counter = some_counter + 1;
        end
    end
    
    % Apply input
    u_input = min([max([u_input, -100]), 100]); % Input saturation
    motor_controller.send('u', [u_input, r_local]);
    
    
    %disp([t_n, u_input, u_n, y_n, r_local]);
end

% Disable devices
motor_controller.send('u', [0, 0]);
for device = devices
    device.send_signals = false;
end
pause(0.1); % wait for remaining messages to arrive.
disp('done');

t_n = motor_controller.get_signals('t');
y_n = motor_controller.get_signals('y');
r_n = motor_controller.get_signals('r');
u_n = motor_controller.get_signals('u');

e_n = r_n-y_n;
r_width = max(r_n)-min(r_n);
percentage_std = 100*std(e_n)/r_width;
percentage_mean = 100*mean(e_n)/r_width;

disp(['mean_error_variation = ', num2str(percentage_mean), '% of max r-variation']);
disp([' std_error_variation = ', num2str(percentage_std), '% of max r-variation']);

figure(1);
subplot(2,1,1);
plot(t_n, y_n, t_n, r_n);
legend('y', 'r');

subplot(2,1,2);
plot(t_n, u_n);
legend('u');

%% Read sensors, move "function" into section
clear;
my_sensor_band = SppBluetooth('0877A9E350CC@Exo-Aider', 'my_left_sensor_band_task', 60 * 1000); % buffer size at the end

    my_sensor_band.sample_frequency = 1000;
    my_sensor_band.send_signals_ratio = 40;
    my_sensor_band.send_signals = true;
    my_sensor_band.flush;
    pause(1);
    signals_got = min(my_sensor_band.signal_n);
    % my_sensor_band.send_signals = false;
    
FSR = my_sensor_band.get_signals({'FSR1','FSR2','FSR3','FSR4','FSR5','FSR6','FSR7','FSR8'});
EMG = my_sensor_band.get_signals({'EMG1','EMG2','EMG3','EMG4'});
% External IMU1
IMU1 = my_sensor_band.get_signals({'AccX1', 'AccY1', 'AccZ1', 'GyroX1', 'GyroY1', 'GyroZ1'}); 
% Internal IMU2
IMU2 = my_sensor_band.get_signals({'AccX2', 'AccY2', 'AccZ2', 'GyroX2', 'GyroY2', 'GyroZ2'});

%% Real time plotting

x = linspace(0, 60, 60000);

while(size(FSR, 1) < 60000)
    FSR = my_sensor_band.get_signals({'FSR1','FSR2','FSR3','FSR4','FSR5','FSR6','FSR7','FSR8'});
    EMG = my_sensor_band.get_signals({'EMG1','EMG2','EMG3','EMG4'});
    IMU1 = my_sensor_band.get_signals({'AccX1', 'AccY1', 'AccZ1', 'GyroX1', 'GyroY1', 'GyroZ1'}); 
    IMU2 = my_sensor_band.get_signals({'AccX2', 'AccY2', 'AccZ2', 'GyroX2', 'GyroY2', 'GyroZ2'});
end

for i = 1:1000 % plotting in 100 seconds real time
    FSR = my_sensor_band.get_signals({'FSR1','FSR2','FSR3','FSR4','FSR5','FSR6','FSR7','FSR8'});
    EMG = my_sensor_band.get_signals({'EMG1','EMG2','EMG3','EMG4'});
    IMU1 = my_sensor_band.get_signals({'AccX1', 'AccY1', 'AccZ1', 'GyroX1', 'GyroY1', 'GyroZ1'}); 
    IMU2 = my_sensor_band.get_signals({'AccX2', 'AccY2', 'AccZ2', 'GyroX2', 'GyroY2', 'GyroZ2'});
    
    plot(x+i*0.1, EMG);
    xlabel('Seconds')
    
    pause(0.01);
end



%% Simulated signal test (sin()millis())

simSignal = SppBluetooth('F4418CA4AE30@Exo-Aider', 'simulated_signal', 2 * 1000);

    simSignal.sample_frequency = 1000;
    simSignal.send_signals_ratio = 50;
    simSignal.flush;
    simSignal.send_signals = true;
    % Start trial 1
    simSignal.start_log_signals_to_file('tmp_simSignal_1.bin'); 
    pause(6);
    
    % End trial 1
    % simSignal.send_signals = false; % stops sending signals
    simSignal.stop_log_signals_to_file();
    signals_got = min(simSignal.signal_n);
    
    % Start trial 2
    simSignal.start_log_signals_to_file('tmp_simSignal_2.bin'); 
    pause(6);
    
    % End trial 2
    % simSignal.send_signals = false; % stops sending signals
    simSignal.stop_log_signals_to_file();
    signals_got = min(simSignal.signal_n);
    
sineLow = simSignal.get_signals('simSineLow');
sineHigh = simSignal.get_signals('simSineHigh');

t_1 = SppBluetooth.load_signals_from_file('tmp_simSignal_1.bin');
t_2 = SppBluetooth.load_signals_from_file('tmp_simSignal_2.bin');

function r_n = calculate_reference(t, frequency)
    r_n = 10 * sin(2 * pi * frequency * t);
    r_n(r_n<0) = -5;
end






