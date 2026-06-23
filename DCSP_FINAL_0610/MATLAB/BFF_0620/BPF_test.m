% subject: Digital control and signal Processing
% title: Band Pass Filter (BPF) test
% author: Juyoung Lee

% clear; clc; close all;
addpath('..\lib_function\');

fileName = 'Stabil_Dynamics.out';
DAQ_data_values = readmatrix(fileName, 'NumHeaderLines', 1, 'FileType', 'text');

% =========================================================================
% FFT 
% =========================================================================

% [입력] DAQ_data_values: 1열이 시간, 2열부터가 신호인 원본 데이터 Matrix
% [출력] DAQ_FFT_Matrix: 1열이 주파수, 2열부터가 FFT 크기인 결과 Matrix

% 1. 시간 축 파라미터 계산 (1열 데이터 활용)
DAQ_time            = DAQ_data_values(:, 1);
DAQ_length          = length(DAQ_time);
DAQ_dt              = DAQ_time(2) - DAQ_time(1); % 샘플링 주기
DAQ_time_uniform    = linspace(DAQ_time(1), DAQ_time(end), DAQ_length)';
DAQ_Wgyro_data      = DAQ_data_values(:, 4);
DAQ_Wb_data         = DAQ_data_values(:, 6);

fft_sampling        = 1000;                % 샘플링 주파수

[DAQ_freq_axis, DAQ_fft_positive] = FFT_1D(DAQ_time_uniform, DAQ_Wgyro_data,fft_sampling);

% figure(5); hold on;
% plot(DAQ_time_uniform, DAQ_Wgyro_data, 'b');
% title(fileName);

figure(2);
plot(DAQ_freq_axis, DAQ_fft_positive, 'r');
title(fileName);

% =========================================================================
% BPF
% =========================================================================
% analoge
Wc                              = 0.5*2*pi;

Wgyro_BPF                       = BPF_1D(DAQ_time_uniform, DAQ_Wgyro_data, Wc, Wc/5);
Wgyro_LPF                       = LPF_1D(DAQ_time_uniform, DAQ_Wgyro_data, Wc);
Wb_BPF                          = BPF_1D(DAQ_time_uniform, DAQ_Wb_data, Wc, Wc/5);

[DAQ_freq_axis, fft_Wgyro_BPF ] = FFT_1D(DAQ_time_uniform, Wgyro_BPF,fft_sampling);
[~, fft_Wgyro_LPF]              = FFT_1D(DAQ_time_uniform, Wgyro_LPF,fft_sampling);

[~, fft_Wb_BPF]                 = FFT_1D(DAQ_time_uniform, Wb_BPF,fft_sampling);

% BPF is more proper than LPF
figure(5); 
plot(DAQ_time_uniform,Wgyro_BPF, 'r',DAQ_time_uniform, Wgyro_LPF, 'b',LineWidth=1.2);

figure(6);
plot(DAQ_freq_axis, fft_Wgyro_BPF, 'r', DAQ_freq_axis, fft_Wgyro_LPF, 'b');