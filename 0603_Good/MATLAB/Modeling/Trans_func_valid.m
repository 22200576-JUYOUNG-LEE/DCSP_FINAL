function Trans_func_valid(path)
%% Motor Validation with Quantitative Error Metrics

[sys_final, g_initial] = Trans_func(path);

% --- Storage for summary table ---
freq_list   = [];
rmse_list   = [];
nrmse_list  = [];
amp_ratio_list  = [];
phase_err_list  = [];

for f_val = 0.2:0.2:2.0
    validation_fn = sprintf('%sBodeMag_sin_freq%.2f.out', path, f_val);
    if ~isfile(validation_fn), continue; end

    %% 1. Read raw data
    val_data = readmatrix(validation_fn, 'FileType', 'text', 'NumHeaderLines', 1);
    t_old = val_data(:, 1);
    u_raw = val_data(:, 2);   % Vcmd [V]
    y_raw = val_data(:, 5);   % Wgyro [deg/s]
    
    t_raw = linspace(t_old(1), t_old(end), length(u_raw));
        

    %% 2. Simulate
    y_sim = lsim(sys_final, u_raw, t_raw) * g_initial;

    %% 3. Trim transient (first 20% of signal)
    % The model starts from zero; real data may have initial offset.
    % Comparing only the steady-state portion is fairer.
    n      = length(t_raw);
    i_ss   = round(0.2 * n) + 1 : n;   % steady-state index range
    t_ss   = t_raw(i_ss);
    y_r    = y_raw(i_ss);
    y_s    = y_sim(i_ss);

    %% 4. Time-domain metrics
    err     = y_r - y_s;
    rmse    = sqrt(mean(err.^2));
    nrmse   = (1 - norm(err) / norm(y_r - mean(y_r))) * 100;  % MATLAB "fit %"

    %% 5. Frequency-domain metrics (FFT at excitation frequency)
    dt   = mean(diff(t_ss));
    Fs   = 1 / dt;
    N    = length(t_ss);
    f_ax = (0:N-1) * (Fs / N);

    Y_r  = fft(y_r) / N;
    Y_s  = fft(y_s) / N;

    % Find FFT bin closest to excitation frequency
    [~, idx] = min(abs(f_ax - f_val));

    amp_real  = 2 * abs(Y_r(idx));
    amp_sim   = 2 * abs(Y_s(idx));
    amp_ratio = amp_sim / amp_real;          % 1.0 = perfect amplitude match

    phase_real = angle(Y_r(idx)) * 180/pi;
    phase_sim  = angle(Y_s(idx)) * 180/pi;
    phase_err  = phase_sim - phase_real;     % deg; negative = model lags

    % Wrap to [-180, 180]
    phase_err = mod(phase_err + 180, 360) - 180;

    %% 6. Store for summary
    freq_list      = [freq_list;      f_val];
    rmse_list      = [rmse_list;      rmse];
    nrmse_list     = [nrmse_list;     nrmse];
    amp_ratio_list = [amp_ratio_list; amp_ratio];
    phase_err_list = [phase_err_list; phase_err];

    %% 7. Plot: time-domain with error subplot
    figure('Name', sprintf('Validation: %.2f Hz', f_val), ...
           'Position', [100 100 900 600]);

    % --- Top: signal comparison ---
    subplot(2,1,1);
    plot(t_ss, y_r, 'k-',  'LineWidth', 1.2); hold on;
    plot(t_ss, y_s, 'r--', 'LineWidth', 1.5);
    grid on;
    title(sprintf('Frequency: %.2f Hz  |  NRMSE fit = %.1f%%  |  Amp ratio = %.3f  |  Phase err = %.1f deg', ...
                  f_val, nrmse, amp_ratio, phase_err));
    xlabel('Time (s)'); ylabel('\omega_{gyro} [deg/s]');
    legend('Real', 'Model', 'Location', 'best');

    % --- Bottom: error signal ---
    subplot(2,1,2);
    plot(t_ss, err, 'b-', 'LineWidth', 1.0);
    yline(rmse,  'r--', sprintf('RMSE = %.2f deg/s', rmse), 'LineWidth', 1.2);
    yline(-rmse, 'r--', '', 'LineWidth', 1.2);
    yline(0, 'k:', 'LineWidth', 0.8);
    grid on;
    title('Error: Real - Model');
    xlabel('Time (s)'); ylabel('Error [deg/s]');
end

%% 8. Summary table
fprintf('\n====== Validation Summary ======\n');
fprintf('%-8s %-10s %-12s %-14s %-14s\n', ...
        'Freq(Hz)', 'RMSE', 'NRMSE fit%', 'Amp Ratio', 'Phase Err(deg)');
fprintf('%s\n', repmat('-', 1, 60));
for k = 1:length(freq_list)
    fprintf('%-8.2f %-10.2f %-12.1f %-14.3f %-14.1f\n', ...
            freq_list(k), rmse_list(k), nrmse_list(k), ...
            amp_ratio_list(k), phase_err_list(k));
end

%% 9. Summary plot: metrics vs frequency
figure('Name', 'Validation Metrics Summary', 'Position', [100 100 900 700]);

subplot(3,1,1);
plot(freq_list, nrmse_list, 'bo-', 'LineWidth', 1.5, 'MarkerFaceColor', 'b');
yline(80, 'k--', '80% threshold', 'FontSize', 9);
grid on;
ylabel('NRMSE Fit (%)'); xlabel('Frequency (Hz)');
title('Model Fit Quality vs. Frequency');
ylim([0 100]);

subplot(3,1,2);
plot(freq_list, amp_ratio_list, 'rs-', 'LineWidth', 1.5, 'MarkerFaceColor', 'r');
yline(1.0, 'k--', 'ideal', 'FontSize', 9);
grid on;
ylabel('Amplitude Ratio (sim/real)'); xlabel('Frequency (Hz)');
title('Amplitude Match (1.0 = perfect)');

subplot(3,1,3);
plot(freq_list, phase_err_list, 'g^-', 'LineWidth', 1.5, 'MarkerFaceColor', 'g');
yline(0, 'k--', 'ideal', 'FontSize', 9);
grid on;
ylabel('Phase Error (deg)'); xlabel('Frequency (Hz)');
title('Phase Error: Model - Real (negative = model lags)');