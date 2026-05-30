function [sys_final, g_initial] = Trans_func(path)
%% 1. Get Data
[f_exp, g_exp, p_exp] = generate_bode_plot(path);
g_norm   = g_exp / g_exp(1);
g_initial = g_exp(1);

%% 2. Define Fit Range
f_min = 0.1;
f_max = 1.0;
idx = find(f_exp >= f_min & f_exp <= f_max);
w   = 2 * pi * f_exp(idx);
p_rad = p_exp(idx) * pi / 180;

%% 3. Phase-only Fitting (2nd order, lsqcurvefit)
% model: phi(w) = -atan2(2*zeta*wn*w, wn^2 - w^2)
phase_model = @(params, w) -atan2( 2 * params(2) * params(1) .* w, (params(1)^2 - w.^2));

% initial guess from data
[~, idx90] = min(abs(p_exp(idx) - (-90)));
wn0   = w(idx90); %bandwidth 
zeta0 = 70; %0.5  
params0 = [wn0, zeta0];


%options = optimoptions('lsqcurvefit', 'Display', 'off');
params_est = lsqcurvefit(phase_model, params0, w, p_rad);

wn_est   = params_est(1);
zeta_est = params_est(2);
fn_est   = wn_est / (2 * pi);

% 2nd order transfer function
% H(s) = wn^2 / (s^2 + 2*zeta*wn*s + wn^2)
num_fit = wn_est^2;
den_fit = [1, 2*zeta_est*wn_est, wn_est^2];
sys_phase = tf(num_fit, den_fit);

%% 4. invfreqs 
h       = g_norm(idx) .* exp(1j * p_rad);
[num_inv, den_inv] = invfreqs(h, w, 0, 2);
sys_inv = tf(num_inv, den_inv);

%% 5. 최종 시스템 선택 — phase fitting 기준
sys_final = sys_phase;

%% 6. Report
fprintf('\n--- Phase Fitting Result ---\n');
fprintf('wn   = %.4f rad/s = %.4f Hz\n', wn_est, fn_est);
fprintf('zeta = %.4f\n', zeta_est);
fprintf('DC gain = %.4f\n', g_initial);
fprintf('\n--- invfreqs Result (reference) ---\n');
printsys(num_inv, den_inv, 's');

%% 7. Visualization
figure('Name', 'System ID Results');

% Magnitude
subplot(2,1,1); hold on; grid on;
semilogx(f_exp, 20*log10(g_norm),          'k.', 'MarkerSize', 12, 'DisplayName', 'Measured');
[m_phase, ~] = bode(sys_phase, 2*pi*f_exp);
semilogx(f_exp, 20*log10(squeeze(m_phase)), 'r-', 'LineWidth', 2,  'DisplayName', sprintf('Phase fit: wn=%.2fHz, zeta=%.3f', fn_est, zeta_est));
[m_inv, ~]   = bode(sys_inv,   2*pi*f_exp);
semilogx(f_exp, 20*log10(squeeze(m_inv)),   'b--','LineWidth', 1.5, 'DisplayName', 'invfreqs');
ylabel('Magnitude (dB)');
title('Experimental vs Fitted');
legend('Location', 'southwest');

% Phase
subplot(2,1,2); hold on; grid on;
semilogx(f_exp, p_exp,                      'k.', 'MarkerSize', 12, 'DisplayName', 'Measured');
[~, p_phase] = bode(sys_phase, 2*pi*f_exp);
semilogx(f_exp, squeeze(p_phase),            'r-', 'LineWidth', 2,  'DisplayName', 'Phase fit');
[~, p_inv]   = bode(sys_inv,   2*pi*f_exp);
semilogx(f_exp, squeeze(p_inv),              'b--','LineWidth', 1.5, 'DisplayName', 'invfreqs');
ylabel('Phase (deg)');
xlabel('Frequency (Hz)');
legend('Location', 'southwest');

%% 8. Pole-Zero Map
figure('Name', 'Pole-Zero Map');
pzmap(sys_phase, sys_inv);
legend('Phase fit', 'invfreqs');
end