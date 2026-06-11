% subject: Digital control
% title: Designation Loop
% author: Jihoon Park
clc;
clear all;
close all;

filename = '..\..\Data\Designation_data\Designation_Step_30.out'; 
data = readmatrix(filename, 'FileType', 'text', 'NumHeaderLines', 1);

%% Motor parameters
Pm = 10.42;
Km = 1301;

%% Pole placement - tune these two
Wn   = 38;
zeta = 1/sqrt(2);

%% Controller gains (auto-calculated)
Kp = Wn^2 / (Km * Pm);
Kd = (2*zeta*Wn - Pm) / (Km * Pm);

fprintf('Kp = %.6f\n', Kp);
fprintf('Kd = %.6f\n', Kd);
fprintf('Kp/Kd (-K- block) = %.4f\n', Kp/Kd);

%% Step input for simin
Tf   = 2;
Ts   = 0.005;
time = (0:Ts:Tf)';
input_val=30.0;
data_time = data(:,1);
data_theta   = data(:, 4); % Actual Angle [deg] 

simin_theta = [data_time data_theta];
simin_step = [time, input_val*ones(size(time))];   % 1 deg step

%% Run Simulink
out = sim("Designation.slx");

%% Plot
t   = out.simout_time;
sim_psi = out.simout_Angle;
real_psi = out.simout_real_Angle;
rate_motor = out.simout_Motor_rate;

fft_rate = fft(rate_motor);


data_size = length(t);
ninety_percent = zeros(size(t));
for i = 1:1:data_size
    ninety_percent(i) = sim_psi(end) * 0.9;
end

figure(1), clf;
hold on;
plot(t, ones(size(t)),     'Color', [0.8500 0.3250 0.0980], 'LineWidth', 1.5, 'LineStyle', '--');
plot(t, sim_psi,               'Color', [0.0000 0.4470 0.7410], 'LineWidth', 2.0);
plot(t, real_psi,           'Color', [0.8500 0.0000 0.0980], 'LineWidth', 2.0);
plot(t, ninety_percent,    ':', 'Color', [0.4660 0.6740 0.1880], 'LineWidth', 1.5);
grid on, box on;
xlabel('time [sec]');
ylabel('response [deg]');
legend('\psi_{gc}', '\psi_g', '\psi_{actual}', '90%', 'Location', 'best');
title(sprintf('Designation Loop  (W_n=%d rad/s,  \\zeta=%.1f)', Wn, zeta));

%% ==========================================
%% 2. 마진 계산을 위한 시스템 정의 (주파수 영역)
%% ==========================================
s = tf('s');

% 1) 모터 플랜트 및 센서 통합 모델 (Rate Gyro 적분 파트 포함)
% 요약 슬라이드 구조 반영: G_motor = Km / (s*(s + Pm))
G_plant = Km / (s * (s + Pm));

% 2) 제어기 구조 (PD 제어기 형태: Kp + Kd*s)
G_controller = Kp + Kd * s;

% 3) 개루프 전달함수 (Open-loop Transfer Function: L(s))
Go = G_controller * G_plant;
Gcl = Go / (1+ Go);

Gcl_min = minreal(Gcl);

figure();
nyquist(Go);
figure();
pzmap(Gcl_min);

% 4) 이득 여유(GM) 및 위상 여유(PM) 추출
[Gm, Pm_val, Wcg, Wcp] = margin(Go);
Gm_dB = 20*log10(Gm); % dB 단위 변환

%% ==========================================
%% 3. 스펙 검증 및 PASS/FAIL 판정
%% ==========================================
input_val = 80.0; 

% 시간 영역 계산
target_90 = sim_psi(end) * 0.9;
idx_tr = find(sim_psi >= target_90, 1, 'first');

if ~isempty(idx_tr)
    tr_90 = t(idx_tr); 
else
    tr_90 = Inf; 
end

max_response = max(sim_psi);
overshoot_pct = ((max_response - sim_psi(end)) / sim_psi(end)) * 100;
if overshoot_pct < 0, overshoot_pct = 0; end
ess = abs(input_val - sim_psi(end));

% 스펙 한계치 설정 (슬라이드 기준)
spec_tr_limit   = 0.1;   % sec
spec_os_limit   = 10.0;  % %
spec_ess_limit  = 5.0;   % deg
spec_pm_limit   = 45.0;  % deg
spec_gm_limit   = 10.0;  % dB

% 개별 조건 만족 여부 (Boolean)
pass_tr  = (tr_90 <= spec_tr_limit);
pass_os  = (overshoot_pct < spec_os_limit);
pass_ess = (ess <= spec_ess_limit);
pass_pm  = (Pm_val >= spec_pm_limit);
pass_gm  = (Gm_dB >= spec_gm_limit || isinf(Gm_dB)); % 2차 시스템 계열은 GM이 무한대(Inf)일 수 있음

% 최종 종합 판정 (5개 조건 모두 만족해야 PASS)
final_pass = pass_tr && pass_os && pass_ess && pass_pm && pass_gm;

%% ==========================================
%% 4. 최종 결과 리포트 출력
%% ==========================================
fprintf('\n============= SPECIFICATION REPORT =============\n');
fprintf('[Time Domain Evaluation]\n');
fprintf('1. Rise Time (tr^90) : %.4f sec \t[%s] (Criteria <= %.1f s)\n', tr_90, getStatusStr(pass_tr), spec_tr_limit);
fprintf('2. Max Overshoot(%%OS): %.2f %%    \t[%s] (Criteria <  %.1f %%)\n', overshoot_pct, getStatusStr(pass_os), spec_os_limit);
fprintf('3. Steady-State Error: %.4f deg \t[%s] (Criteria <= %.1f deg)\n', ess, getStatusStr(pass_ess), spec_ess_limit);

fprintf('\n[Frequency Domain Evaluation]\n');
fprintf('4. Phase Margin (PM) : %.2f deg   \t[%s] (Criteria >= %.1f deg)\n', Pm_val, getStatusStr(pass_pm), spec_pm_limit);
if isinf(Gm_dB)
    fprintf('5. Gain Margin (GM)  : Inf dB     \t[%s] (Criteria >= %.1f dB)\n', getStatusStr(pass_gm), spec_gm_limit);
else
    fprintf('5. Gain Margin (GM)  : %.2f dB    \t[%s] (Criteria >= %.1f dB)\n', Gm_dB, getStatusStr(pass_gm), spec_gm_limit);
end

fprintf('------------------------------------------------\n');
if final_pass
    fprintf('★ FINAL DESIGN RESULT: [ PASS ] ★\n');
else
    fprintf('❌ FINAL DESIGN RESULT: [ FAIL ] ❌\n');
end
fprintf('================================================\n');

% %% 그래프 상에 텍스트 표시
% text_x = Tf * 0.4;
% if final_pass
%     text(text_x, 0.4, 'TOTAL SPEC: PASS', 'Color', [0 0.5 0], 'FontSize', 12, 'FontWeight', 'bold', 'EdgeColor', [0 0.5 0]);
% else
%     text(text_x, 0.4, 'TOTAL SPEC: FAIL', 'Color', [1 0 0], 'FontSize', 12, 'FontWeight', 'bold', 'EdgeColor', [1 0 0]);
% end

%% 주파수 응답 확인용 마진 보드선도 추가 가동 (선택 사항)
figure('Name', 'Open-Loop Margin Plot');
margin(Go);

%% 관련 헬퍼 함수
function str = getStatusStr(pass_flag)
    if pass_flag, str = 'PASS'; else, str = 'FAIL'; end
end