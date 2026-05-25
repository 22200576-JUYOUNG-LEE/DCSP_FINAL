function [sys_final, g_initial]=Trans_func(path)
%% 1. Get Data

[f_exp, g_exp, p_exp] = generate_bode_plot(path);



g_norm = g_exp / g_exp(1);

g_initial=g_exp(1); 

%% 2. Define Fit Range (1/10 to 1/3 of 4Hz Bandwidth)
f_min = 1.2; 
f_max = 4.0;
idx = find(f_exp >= f_min & f_exp <= f_max);


h = g_norm(idx) .* exp(1j * p_exp(idx) * pi/180);
w = 2 * pi * f_exp(idx);

%% 3. System Identification (invfreqs)

[num, den] = invfreqs(h, w, 1, 2);
sys_final = tf(num, den);

figure;
pzmap(sys_final);


%% 4. Reporting & Visualization
fprintf('\n--- Identified Transf5er Function ---\n');
printsys(num, den, 's'); % Nicer display format
[wn, zeta] = damp(sys_final);

figure('Name', 'System ID Results');
subplot(2,1,1);
semilogx(f_exp, 20*log10(g_norm), 'k.', 'MarkerSize', 12); hold on;
[m_fit, ~] = bode(sys_final, 2*pi*f_exp);
% semilogx(f_exp, 20*log10(squeeze(m_fit)), 'r-', 'LineWidth', 2);
grid on; ylabel('Mag (dB)'); title('Experimental Data vs invfreqs Fit');

subplot(2,1,2);
semilogx(f_exp, p_exp, 'k.', 'MarkerSize', 12); hold on;
[~, p_fit] = bode(sys_final, 2*pi*f_exp);
% semilogx(f_exp, squeeze(p_fit), 'b-', 'LineWidth', 2);
grid on; ylabel('Phase (deg)'); xlabel('Frequency (Hz)');

