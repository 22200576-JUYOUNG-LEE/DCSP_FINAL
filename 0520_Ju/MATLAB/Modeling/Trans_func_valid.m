function Trans_func_valid(path)

%% Motor Validation: 

[sys_final, g_initial]=Trans_func(path);


for f_val = 0.4:0.2:2.0
    validation_fn = sprintf('%sBodeMag_sin_freq%.2f.out', path, f_val);
    if ~isfile(validation_fn), continue; end
    
    % 1. Read raw data
    val_data = readmatrix(validation_fn, 'FileType', 'text', 'NumHeaderLines', 1);
    t_raw = val_data(:, 1);
    u_raw = val_data(:, 2); %Vcmd [V]
    y_raw = val_data(:, 5); %Wgyro [deg/sec]  
    
    % 2. Simulate response
    y_sim = lsim(sys_final, u_raw, t_raw) * g_initial;
    
    % 3. Plot sequentially
    figure('Name', sprintf('Validation: %.2f Hz', f_val));
    plot(t_raw, y_raw, 'k-', 'LineWidth', 1.2); hold on;
    plot(t_raw, y_sim, 'r--', 'LineWidth', 1.5);
    grid on; 
    title(sprintf('Frequency: %.2f Hz', f_val));
    xlabel('Time (s)'); ylabel('\omega_{gyro}');
    legend('Real', 'Model');
end