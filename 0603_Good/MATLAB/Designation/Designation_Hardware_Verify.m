% Designation_Hardware_Verify.m
clc; clear; close all;

%% 1. Hardware
filename = '..\..\Data\Designation_data\Designation_Step_30.out'; 
data = readmatrix(filename, 'FileType', 'text', 'NumHeaderLines', 1);

t       = data(:, 1); % Time [sec]
theta   = data(:, 4); % Actual Angle [deg] 
    
target_deg = 80.0; % modify here manually. 

%% 2. spec (noise)
% 1) 90% rising time (Rise Time to 90%)
target_90 = target_deg * 0.9;
idx_tr = find(theta >= target_90, 1, 'first');
if ~isempty(idx_tr)
    tr_90 = t(idx_tr);
else
    tr_90 = Inf;
end

% 2)  (% Overshoot)
max_response = max(theta);
overshoot_pct = ((max_response - target_deg) / target_deg) * 100;
if overshoot_pct < 0, overshoot_pct = 0; end

% 3) (Steady-State Error)
% for noise consideration 
idx_ss = find(t >= (t(end) - 0.5));
ss_val = mean(theta(idx_ss));
ess = abs(target_deg - ss_val);

%% 3. spec validation 
spec_tr_limit   = 0.1;   % sec
spec_os_limit   = 10.0;  % %
spec_ess_limit  = 5.0;   % deg

pass_tr  = (tr_90 <= spec_tr_limit);
pass_os  = (overshoot_pct < spec_os_limit);
pass_ess = (ess <= spec_ess_limit);

final_pass = pass_tr && pass_os && pass_ess;

%% 4. 결과 플롯
figure('Name', 'Hardware Designation Loop Validation', 'Position', [100, 100, 700, 500]);
hold on; grid on; box on;

% 궤적 플롯
plot(t, target_deg * ones(size(t)), 'Color', [0.8500 0.3250 0.0980], 'LineWidth', 2, 'LineStyle', '--');
plot(t, theta, 'Color', [0.0000 0.4470 0.7410], 'LineWidth', 1.5);
plot(t, target_90 * ones(size(t)), ':', 'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1.5);

% X, Y축 제한 및 라벨링
xlim([0 t(end)]);
ylim([0 target_deg * 1.3]);
xlabel('Time [sec]');
ylabel('Response Angle [deg]');
legend('Command (\psi_{gc})', 'Hardware Response (\psi_g)', '90% Target', 'Location', 'southeast');
title('Hardware Designation Loop Step Response');

% 그래프 안에 결과 텍스트 박스 삽입
result_str = sprintf('Rise Time: %.4f s\nOvershoot: %.2f %%\nSS Error: %.2f deg', tr_90, overshoot_pct, ess);
dim = [0.15 0.7 0.3 0.2];
if final_pass
    annotation('textbox', dim, 'String', {'[ DESIGN: PASS ]', result_str}, 'FitBoxToText', 'on', 'Color', 'g', 'FontWeight', 'bold', 'BackgroundColor', 'w');
else
    annotation('textbox', dim, 'String', {'[ DESIGN: FAIL ]', result_str}, 'FitBoxToText', 'on', 'Color', 'r', 'FontWeight', 'bold', 'BackgroundColor', 'w');
end

%% 5. 
fprintf('\n============= HARDWARE SPEC REPORT =============\n');
fprintf('1. Rise Time (tr^90) : %.4f sec \t[%s] (Limit <= %.1f s)\n', tr_90, getStatusStr(pass_tr), spec_tr_limit);
fprintf('2. Max Overshoot(%%OS): %.2f %%    \t[%s] (Limit <  %.1f %%)\n', overshoot_pct, getStatusStr(pass_os), spec_os_limit);
fprintf('3. Steady-State Error: %.4f deg \t[%s] (Limit <= %.1f deg)\n', ess, getStatusStr(pass_ess), spec_ess_limit);
fprintf('------------------------------------------------\n');
if final_pass
    fprintf(' FINAL HARDWARE RESULT: [ PASS ] \n');
else
    fprintf('FINAL HARDWARE RESULT: [ FAIL ] \n');
end
fprintf('================================================\n');

%% 헬퍼 함수
function str = getStatusStr(pass_flag)
    if pass_flag, str = 'PASS'; else, str = 'FAIL'; end
end