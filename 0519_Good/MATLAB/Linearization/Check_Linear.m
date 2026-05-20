% Load both datasets
file_you = '..\data\static\Linear_0512_2.out';

% Assuming they are identically structured
data_you = readmatrix(file_you, 'NumHeaderLines', 1, 'FileType', 'text');

V_you = data_you(:,1); W_you = data_you(:,2);

% --- Calculate Fits for "You" data (Thresholds: 0.05 / -0.05) ---
p_mask_y = V_you >= 0.05;  n_mask_y = V_you <= -0.05;
p_slope_y = V_you(p_mask_y) \ W_you(p_mask_y);
n_slope_y = V_you(n_mask_y) \ W_you(n_mask_y);

% Extended axes for plotting deadzone reference lines
V_ext_pos = linspace(0, 0.25, 100);
V_ext_neg = linspace(-0.25, 0, 100);

% --- Create Subplots ---
figure('Position', [100, 100, 1000, 450]);

% Left Subplot: Linear_test_you
hold on; grid on;
plot(V_you, W_you, 'o-', 'LineWidth', 1.5, 'MarkerFaceColor', 'b', 'DisplayName', 'Original Data');
plot(V_ext_pos, p_slope_y * V_ext_pos, '--r', 'LineWidth', 2, 'DisplayName', sprintf('Pos Fit (Slope: %.2f)', p_slope_y));
plot(V_ext_neg, n_slope_y * V_ext_neg, '--g', 'LineWidth', 2, 'DisplayName', sprintf('Neg Fit (Slope: %.2f)', n_slope_y));
xline(0, 'k--', 'HandleVisibility', 'off'); yline(0, 'k--', 'HandleVisibility', 'off');
plot(0, 0, 'kx', 'MarkerSize', 10, 'LineWidth', 2, 'DisplayName', 'Origin');
title('Linear\_test\_you.out');
xlabel('Vin [V]'); ylabel('Wgyro\_avg [rad/s]');
legend('Location', 'northwest');
hold off;
