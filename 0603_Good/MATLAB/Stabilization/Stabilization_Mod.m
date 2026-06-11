% subject: Digital control and signal Processing
% title: Linearization

clc;
clear all;
close all;

Tstep   = 0.001;    %[sec] for looking 0.005 [sec] sampling effect
Tsample = 0.005;    %[sec] 
Tf      = 10;        %[sec]
time    = 0:Tstep:Tf;

s       = tf('s');

% motor property
Km          = 1     ;   % [rad/(sec*V)]
Time_const  = 0.0573;   % [sec]

Gm          = Km / (Time_const*s + 1); % Motor bandwidth 2.7723 [Hz]

%==========================================================================
% W_cl iteration for getting insight
%==========================================================================
% 1. Closed loop Values -> 3rd order system
Zeta_cl     = 1/sqrt(2);    % [-]
% figure(1); hold on;
% for iter = 1:1:10
%     W_cl        = 18 + iter;         % [rad/s]
% 
%     % 2. tf of Gc
%     Kp          = (2*W_cl*Zeta_cl*Time_const - 1)/Km; % [-]
%     Ki          = (Time_const/Km)*W_cl^2;             % [V/rad] 
%     Gc          = Kp+Ki/s;
% 
%     % 3. System tf
%     Go              = Gc*Gm;
%     Gcl             = Go/(1+Go);
% 
%     % 4. Margin
%     [GM, PM, ~, ~]  = margin(Go);
%     GM_dB = 20 * log10(GM);
% 
%     % 5. Pzmap
%     pz_label = sprintf('W_{cl}:%.2f[rad/s], GM: %.2f dB, PM: %.2f^o', W_cl, GM_dB, PM);
%     pz_Gcl = minreal(Gcl);
%     pz_Gcl.Name = pz_label;
%     pzmap(pz_Gcl);
%     legend; grid on;
% 
% end
% hold off;

%==========================================================================
% Simulink Values
%==========================================================================
W_cl        = 26;         % [rad/s]
    
% 2. tf of Gc
Kp          = (2*W_cl*Zeta_cl*Time_const - 1)/Km; % [-]
Ki          = (Time_const/Km)*W_cl^2;             % [V/rad] 

% 3. Other Values
Sim_disturbance_amplitude   = 8 ;  % [rad/s]
Sim_disturbance_freq        = pi;  % [rad/s] = 0.5[Hz]

Sim_deadZone_start          = -0.5; % [rad/s]
Sim_deadZone_final          = 0.5;  % [rad/s]

Sim_saturation_neg          = -20;   % [rad/s]
Sim_saturation_pos          = 20;   % [rad/s]