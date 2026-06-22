% subject: Digital control and signal Processing
% title: Motor Modeling
clear all; 
clc; 

data_dir        = '..\data\BodeMag\';
search_freqs    = 0.3:0.01:1.3; 
    
saved_freqs         = [];
saved_Wc            = [];
saved_gains         = [];
saved_phases        = [];
saved_freq_response = [];

sampling_time   = 0.005;
sampling_freq   = 200.0;

for idx = 1:length(search_freqs)
    freq            = search_freqs(idx);
    file_name       = sprintf('%sBodeMag_sin_freq%.2f.out', data_dir, freq);
    
    if ~isfile(file_name), continue; end
    
    %----------------------------------------------------------------------
    % Import Bode Mag Data
    %----------------------------------------------------------------------
    data    = readmatrix(file_name, 'FileType', 'text', 'NumHeaderLines', 1);

    % 1. uniform time
    time_raw        = data(1:end, 1);
    time_length     = length(time_raw);
    time_uniform    = linspace(time_raw(1), time_raw(end), time_length)';

    Wcmd_raw    = data(1:end, 2); 
    Wgyro_raw   = data(1:end, 4); 

    %----------------------------------------------------------------------
    % filtering by Band pass filter
    %----------------------------------------------------------------------
    Wc          = freq*2*pi;
    passBand    = Wc / 1.5;
    Wc_range    = [Wc-passBand; Wc+passBand];

    [CT_LPF_NUM, CT_LPF_DEN]    = butter(1, Wc_range, 'band', 's');
    Hs                          = tf(CT_LPF_NUM, CT_LPF_DEN);

    Wcmd        = lsim(Hs, Wcmd_raw, time_uniform);
    Wgyro       = lsim(Hs, Wgyro_raw, time_uniform);
    
    %----------------------------------------------------------------------
    % plotting filtered siganl
    %----------------------------------------------------------------------
    figure(idx); hold on;
    plot(time_uniform, Wcmd,  'b-', 'LineWidth', 1.5, 'DisplayName', '\omega_{cmd}');
    plot(time_uniform, Wgyro, 'r-', 'LineWidth', 1.5, 'DisplayName', '\omega_{gyro}');

    xlabel('Time[sec]','FontWeight','bold');
    ylabel('\omega_{cmd BPF}[rad/sec] vs \omega_{gyro BPF}[rad/sec]','FontWeight','bold');

    title(f);
    legend('\omega_{cmd BPF}[rad/sec]', '\omega_{gyro BPF}[rad/sec]');
    hold off; grid on;

    
    %----------------------------------------------------------------------
    % Get frequency response
    %----------------------------------------------------------------------
    % Extract the second half for steady-state analysis
    time_half           = time_uniform(time_length/2: end);
    Wcmd_half           = Wcmd(time_length/2: end);
    Wgyro_half          = Wgyro(time_length/2: end);

    H_matrix            = [sin(Wc*time_half), cos(Wc*time_half), ones(size(time_half))];

    input_coeffs        = H_matrix \ Wcmd_half;
    input_amp           = norm(input_coeffs(1:2));
    input_phase_rad     = atan2(input_coeffs(2), input_coeffs(1));

    output_coeffs       = H_matrix \ Wgyro_half;
    output_amp          = norm(output_coeffs(1:2));
    output_phase_rad    = atan2(output_coeffs(2), output_coeffs(1));
    
    gain            = output_amp / input_amp;
    phase_rad       = output_phase_rad - input_phase_rad ;
    phase           = mod(phase_rad + pi, 2*pi) - pi;

    freq_response   = gain .* exp(1j * phase);

    %----------------------------------------------------------------------
    % Save frequency response
    %----------------------------------------------------------------------
    saved_freqs         = [saved_freqs;         freq];
    saved_Wc            = [saved_Wc;            Wc];
    saved_gains         = [saved_gains;         gain];
    saved_phases        = [saved_phases;        phase];
    saved_freq_response = [saved_freq_response; freq_response];

end
[num_inv, den_inv]      = invfreqs(saved_freq_response, saved_Wc, 0,2);
sys_inv                 = tf(num_inv, den_inv);
% [mag_inv, phase_inv]    = bode(sys_inv, saved_Wc);

phase_model_1st = @(bw,w) -atan2(w,bw);

[~, idx45] = min(abs(saved_phases + 45));

pm0 = saved_Wc(idx45);

pm_est = lsqcurvefit(phase_model_1st, pm0, saved_Wc, saved_phases);

num_fit_1st             = pm_est;
den_fit_1st             = [1 pm_est];

sys_phase_1st           = tf(num_fit_1st, den_fit_1st);
[mag_inv, phase_inv]    = bode(sys_inv, saved_Wc);

%--------------------------------------------------------------------------
% Bode Plot
%--------------------------------------------------------------------------
figure();
subplot(2,1,1); hold on; grid on;

semilogx(saved_freqs, 20*log10(saved_gains), 'k.', 'MarkerSize', 12, 'DisplayName', 'Measured');
semilogx(saved_freqs, 20*log10(squeeze(mag_inv)), 'b--', 'LineWidth', 1.5, 'DisplayName', 'invfreqs');

ylabel('Magnitude (dB)');
title('Experimental vs Modeling');
legend('Location','southwest');
xlim([saved_freqs(1), saved_freqs(end)]);

subplot(2,1,2); hold on; grid on;
semilogx(saved_freqs, saved_phases, 'k.', 'MarkerSize', 12, 'DisplayName', 'Measured');
semilogx(saved_freqs, squeeze(phase_inv), 'b--', 'LineWidth', 1.5, 'DisplayName', 'invfreqs');

hold off;
