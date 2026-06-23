% subject: Digital control and signal Processing
% title: Validation
clear all; clc; close all;

addpath("lib_function\");
scaler_rad2deg   = 180.0/pi ; 

% -----------------------------------------------------------------
% 1. Import Data
% -----------------------------------------------------------------
base_dir        = '../data/Validation/';
filename        = sprintf('%sValid_%s.out', base_dir, 'tri');
Tri_data        = readmatrix(filename, 'NumHeaderLines', 1, 'FileType', 'text');

Time_tri       = Tri_data(:,1);
Wcmd_tri       = Tri_data(:,2).*scaler_rad2deg;
Wgyro_tri      = Tri_data(:,4).*scaler_rad2deg;

filename        = sprintf('%sValid_%s.out', base_dir, 'sin');
Sin_data        = readmatrix(filename, 'NumHeaderLines', 1, 'FileType', 'text');

Time_sin        = Sin_data(:, 1);
Wcmd_sin        = Sin_data(:, 2).*scaler_rad2deg;
Wgyro_sin       = Sin_data(:, 4).*scaler_rad2deg;

% -------------------------------------------------------------------------
% 2. BPF
% -------------------------------------------------------------------------
LPF_Wc          = 20* 2 * pi;
Wcmd_tri_LPF    = LPF_1D(Time_tri, Wcmd_tri, LPF_Wc);
Wgyro_tri_LPF   = LPF_1D(Time_tri, Wgyro_tri, LPF_Wc);

Wcmd_sin_LPF    = LPF_1D(Time_sin, Wcmd_sin, LPF_Wc);
Wgyro_sin_LPF   = LPF_1D(Time_sin, Wgyro_sin, LPF_Wc);

% -------------------------------------------------------------------------
% 3. FFT
% -------------------------------------------------------------------------
freq_sampling                       = 200.0;
[fft_freq_tri, fft_Wcmd_tri]        = FFT_1D(Time_tri, Wcmd_tri, freq_sampling);
[~,            fft_Wgyro_tri]       = FFT_1D(Time_tri, Wgyro_tri, freq_sampling);

[fft_freq_sin, fft_Wcmd_sin]        = FFT_1D(Time_sin, Wcmd_sin, freq_sampling);
[~,            fft_Wgyro_sin]       = FFT_1D(Time_sin, Wgyro_sin, freq_sampling);

window_size             = 10;
fft_Wcmd_tri            = movmean(fft_Wcmd_tri, window_size, 1);  
fft_Wgyro_tri           = movmean(fft_Wgyro_tri, window_size, 1);

fft_Wcmd_sin            = movmean(fft_Wcmd_sin, window_size, 1);  
fft_Wgyro_sin           = movmean(fft_Wgyro_sin, window_size, 1);

% -------------------------------------------------------------------------
% 4. PLOT
% -------------------------------------------------------------------------
% labeling
label_Wcmd  = '\omega_{cmd}';
label_Wgyro = '\omega_{gyro}';

figure(1); hold on;
plot(Time_tri, Wcmd_tri_LPF, 'r', 'LineWidth', 1.2, 'DisplayName', label_Wcmd);
plot(Time_tri, Wgyro_tri_LPF, 'b', 'LineWidth', 1.2, 'DisplayName', label_Wgyro);
title("Validation triangle signal 0.0083 [Hz]");
ylabel("\omega_{gyro} [rad/s]");
xlabel("\omega_{cmd} [rad/s]");
legend('show', 'Location', 'best');
hold off; grid on;

figure(2); hold on;
plot(Time_sin, Wcmd_sin_LPF, 'r', 'LineWidth', 1.2, 'DisplayName', label_Wcmd);
plot(Time_sin, Wgyro_sin_LPF, 'b', 'LineWidth', 1.2, 'DisplayName', label_Wgyro);
title("Validation sin signal 0.01 [Hz]");
ylabel("\omega_{gyro} [rad/s]");
xlabel("\omega_{cmd} [rad/s]");
legend('show', 'Location', 'best');
hold off; grid on;

figure(3); hold on;
plot(fft_freq_tri, fft_Wcmd_tri, 'r', 'LineWidth', 1.2, 'DisplayName', label_Wcmd);
plot(fft_freq_tri, fft_Wgyro_tri, 'b', 'LineWidth', 1.2, 'DisplayName', label_Wgyro);
title("FFT - Validation triangle signal 0.0083 [Hz]");
ylabel("Magnitude of \omega_{gyro} [dB]");
xlabel("Phase of \omega_{cmd} [Hz]");
legend('show', 'Location', 'best');
hold off; grid on;

figure(4); hold on;
plot(fft_freq_sin, fft_Wcmd_sin, 'r', 'LineWidth', 1.2, 'DisplayName', label_Wcmd);
plot(fft_freq_sin, fft_Wgyro_sin, 'b', 'LineWidth', 1.2, 'DisplayName', label_Wgyro);
title("FFT - Validation sin signal 0.01 [Hz]");
ylabel("Magnitude of \omega_{gyro} [dB]");
xlabel("Phase of \omega_{cmd} [Hz]");
legend('show', 'Location', 'best');
hold off; grid on;
