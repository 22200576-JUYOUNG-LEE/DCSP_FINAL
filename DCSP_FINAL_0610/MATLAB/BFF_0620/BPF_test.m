% subject: Digital control and signal Processing
% title: Band Pass Filter (BPF) test
% author: Juyoung Lee

% clear; clc; close all;

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
DAQ_Wb_data         = DAQ_data_values(:, )

fft_sampling        = 1000;                % 샘플링 주파수

[DAQ_freq_axis, DAQ_fft_positive] = FFT_1D(DAQ_time_uniform, DAQ_Wgyro_data,fft_sampling);

figure(5); hold on;
plot(DAQ_time_uniform, DAQ_Wgyro_data, 'b');
title(fileName);

figure(2);
plot(DAQ_freq_axis, DAQ_fft_positive, 'r');
title(fileName);

% =========================================================================
% BPF
% =========================================================================
% analoge
Wc                              = 0.5*2*pi;

Wgyro_BPF                       = BPF_1D(Time_uniform, DAQ_Wgyro_data, Wc/5);
Wgyro_BPF                       = BPF_1D(Time_uniform, DAQ_)

DAQ_fft_result                  = fft(Wgyro_BPF); % 한 번에 FFT 가능

% 4. FFT 크기(Magnitude) 변환 및 단방향(Positive) 절단
DAQ_fft_mag                     = abs(DAQ_fft_result / DAQ_length);
DAQ_fft_positive                = DAQ_fft_mag(1:floor(DAQ_length/2)+1, :);
DAQ_fft_positive(2:end-1, :)    = 2 * DAQ_fft_positive(2:end-1, :); % DC 성분 제외 2배 보정

figure(5); 
plot(DAQ_time_uniform,Wgyro_BPF, DAQ_time_uniform, BPFed_Wcmd, LineWidth=1.2);

figure(4);
plot(DAQ_freq_axis, DAQ_fft_positive, 'r');
title(fileName);