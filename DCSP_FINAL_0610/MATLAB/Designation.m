% subject: Digital control and signal Processing
% title: Designation
clc; clear all; close all;

%==========================================================================
% init
%==========================================================================
addpath(".\lib_function\");
s                   = tf('s');

target_angle        = 30.0;  %[deg]
scaler_rad2deg      = 180.0/pi ; 
%==========================================================================
% Read Dataset
%==========================================================================
filename        = '..\data\Designation\Designation_Step_without_filter.out'; 
Desig_data         = readmatrix(filename, 'FileType', 'text', 'NumHeaderLines', 1);

Time_raw        = Desig_data(:, 1);
time_length     = length(Time_raw);
time_uniform    = linspace(Time_raw(1), Time_raw(end),time_length);

sampling_period = Time_raw(2) - Time_raw(1);

Angle_deg       = Desig_data(:, 6) * scaler_rad2deg;     % [deg]
Wgyro_deg_sec   = Desig_data(:, 5) * scaler_rad2deg;     % [deg/s]
Filtered_omega  = Desig_data(:, 3) * scaler_rad2deg;     % [deg/s]
norm_Filtered_omega = normalize(Filtered_omega, 'range', [-30 30]);
Step_sig        = target_angle .* ones(time_length,1);   % [deg]


%==========================================================================
% Motor parameter
%==========================================================================
Km          = 11.8676/ 12.6778     ;   % [rad/(sec*V)]
Time_const  = 1 / 12.6778            ;   % [sec]
Gm          = Km / (Time_const*s + 1); % Motor bandwidth 2.7723 [Hz]


%==========================================================================
% W_cl iteration for getting insight
%==========================================================================
% 1. Closed loop Values -> 3rd order system
Zeta_cl     = 1/sqrt(2);    % [-]
W_cl_step   = 3.0;

figure(1); hold on;
figure(2); hold on;
figure(3); hold on;
figure(4); hold on;
for iter = 1:1:1

    % W_cl        = 18 + iter * W_cl_step;         % [rad/s]
    W_cl        = 34 ;

    % 2. tf of Gc
    Kd          = (2*W_cl*Zeta_cl*Time_const - 1)/Km % [-]
    Kp          = (Time_const/Km)*W_cl^2             % [V/rad] 
    Gc          = Kd+Kp/s;

    % 3. System tf
    Go              = Gc*Gm;
    Gcl             = Go/(1+Go);

    % 4. rise time
    Time_90         = 2.6546 / W_cl;  % [sec] 

    % 5. %OS
    OS              = exp(-(Zeta_cl/sqrt(1-Zeta_cl^2))*pi) * 100.0;

    % 5. Margin
    [GM, PM, ~, ~]  = margin(Go);
    GM_dB           = 20 * log10(GM);


    %======================================================================
    % Plot
    %======================================================================
    spec_label      = sprintf(['\\omega_{cl}: %.0f [rad/s], t_{90}: %.2f [sec], ' ...
        '%%OS: %.2f%%, GM: %.2f dB, PM: %.2f^\\circ'], W_cl, Time_90, OS, GM_dB, PM);

    % pz map
    figure(1); 
    pz_Gcl          = minreal(Gcl);
    pz_Gcl.Name     = spec_label;
    pzmap(pz_Gcl);

    % nyquist
    figure(2);
    [re, im, wout]  = nyquist(Go);
    plot(squeeze(re), squeeze(im), LineStyle="--", LineWidth= 1.2, DisplayName=spec_label);

    % Bode 
    figure(3);
    Go.Name         = spec_label;
    bode(Go);

    % % time
    figure(4);
    Gcl_tustin      = c2d(Gcl, sampling_period, 'tustin');
    step_response   = lsim(Gcl_tustin, Step_sig, time_uniform);
    plot(time_uniform, step_response, LineWidth= 1.2, DisplayName=spec_label);


end
figure(1); legend('Interpreter', 'tex', 'Location', 'best');
grid on; hold off;

figure(2);
axis([-1.5 1.5 -1 1]);
ang = 0:0.1:360 ; xc = cosd(ang) ; yc = sind(ang) ;

plot(xc, yc, 'r', 'HandleVisibility', 'off'); legend('Interpreter', 'tex', 'Location', 'best');
grid on; hold off;

figure(3); legend('Interpreter', 'tex', 'Location', 'southwest'); 
grid on; hold off;

figure(4); legend('Interpreter', 'tex', 'Location', 'southwest'); 
grid on; hold off;


%==========================================================================
% Simin Values
%==========================================================================
Tstep           = 0.001;    %[sec] for looking 0.005 [sec] sampling effect
Tsample         = 0.005;    %[sec] 
Tf              = 10;        %[sec]
time            = 0:Tstep:Tf;

simin_Angle         = [Time_raw Angle_deg];
simin_Wgyro         = [Time_raw Wgyro_deg_sec];
simin_step          = [Time_raw Step_sig];


data_Time          = Desig_data(:,1);
data_Wgyro         = Desig_data(:,4);
data_disturbance   = Desig_data(:,5);

target_90       = target_angle * 0.9;

%==========================================================================
% Plot
%==========================================================================

label_Wcmd      = '\psi_{input}';
label_Step      = '\psi_{target}';
label_CMD       = '\psi_{target}';


figure(5); hold on;
plot(Time_raw, Angle_deg, 'r', 'LineWidth', 1.2, 'DisplayName', label_Wcmd);
plot(Time_raw, Step_sig, 'b', 'LineWidth', 1.2, 'DisplayName', label_Step);
plot(Time_raw, norm_Filtered_omega, 'g', 'LineWidth', 1.2, 'DisplayName', label_CMD);
title("time response");
ylabel("\psi_{cmd} vs Step [deg]");
xlabel("time [sec]");
legend('show', 'Location', 'best');
hold off; grid on;
