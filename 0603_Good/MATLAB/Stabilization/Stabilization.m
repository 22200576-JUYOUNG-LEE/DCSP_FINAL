%% ==========================================================
%% 1. w_cl SWEEP (Cleaned & Professional Layout)
%%    Range: 19 ~ w_cl_max 
%% ==========================================================
s = tf('s');
zeta_cl = 1/sqrt(2);

DZ  = 0.35;  
SAT=24.3;

Km=1300; 

%% --- Find upper bound (including digital delay to see real margins) ---
w_test  = 19:1:50; % Expanded test range
w_cl_max = 19;
for w = w_test
    KP_t = (2*w*zeta_cl*time_const - 1)/Km;
    KI_t = (time_const/Km)*w^2;
    Gc_t = KP_t + KI_t/s;
    Gm_t = Km/(time_const*s + 1);
    
    % Including the digital delay (Ts) exposes the true upper bound!
    Delay = (1 - (Ts/2)*s) / (1 + (Ts/2)*s); 
    L_t  = Gc_t * Gm_t * Delay;
    
    [GM_t, PM_t] = margin(L_t);
    if 20*log10(GM_t) >= 10 && PM_t >= 45
        w_cl_max = w;
    else
        break; % Stop sweeping once it fails specifications
    end
end
fprintf('w_cl sweep range: 19 ~ %d [rad/sec]\n', w_cl_max);

w_cl_vec = 19:1:w_cl_max;
N = length(w_cl_vec);

%% --- Clean Gradient Color Map (Light Slate Blue to Deep Navy Blue) ---
% This eliminates the messy rainbow 'jet' effect entirely
c_start = [0.65, 0.75, 0.90]; % Elegant light blue
c_end   = [0.05, 0.15, 0.45]; % Deep professional navy blue
cmap = [linspace(c_start(1), c_end(1), N)', ...
        linspace(c_start(2), c_end(2), N)', ...
        linspace(c_start(3), c_end(3), N)'];

%% --- Nyquist Plot Generation ---
figure('Name','Nyquist - w_cl Sweep','Color','w'); % Crisp white background
hold on;

% 1. Draw the Reference Unit Circle (Muted Gray Dashed)
theta_circle = linspace(0, 2*pi, 300);
plot(cos(theta_circle), sin(theta_circle), 'Color', [0.6 0.6 0.6], 'LineStyle', '--', 'LineWidth', 1.0);

% 2. Plot the Swept Nyquist Curves
for k = 1:N
    w_cl = w_cl_vec(k);
    KP   = (2*w_cl*zeta_cl*time_const - 1)/Km;
    KI   = (time_const/Km)*w_cl^2;
    Gc   = KP + KI/s;
    Gm   = Km/(time_const*s + 1);
    
    % Apply digital sampling delay to match real hardware behavior
    Delay = (1 - (Ts/2)*s) / (1 + (Ts/2)*s);
    L    = Gc * Gm * Delay;
    
    [re, im] = nyquist(L);
    re = squeeze(re);
    im = squeeze(im);
    
    % Smooth gradient plot lines
    plot(re, im, 'Color', cmap(k,:), 'LineWidth', 1.5);
end

% 3. Mark the Critical Stability Point (-1,0) cleanly
plot(-1, 0, 'ro', 'MarkerSize', 7, 'MarkerFaceColor', 'r', 'LineWidth', 1.5);
text(-0.95, 0.1, 'Critical Point (-1,0)', 'FontSize', 9, 'FontWeight', 'bold', 'Color', 'r');

%% --- Formatting and Layout Adjustments ---
xlabel('Real Axis'); ylabel('Imaginary Axis');
title(sprintf('Nyquist Sweep: \\omega_{cl} = 19 ~ %d rad/s (Digital Delay Included)', w_cl_max), 'FontSize', 11);
grid on; 
axis equal;
axis([-2.5 0.5 -2 2]); % Focus closely on the critical zoom zone

% Configure colorbar to match the new professional gradient
colormap(cmap);
cb = colorbar;
cb.Label.String = 'Closed-Loop Bandwidth \omega_{cl} [rad/sec]';
cb.Ticks = linspace(1, N, min(5, N));
cb.TickLabels = string(round(linspace(19, w_cl_max, min(5, N))));
hold off;


%% ==========================================================
%% 3. DESIGN MODEL  (use nominal w_cl = 23)
%% ==========================================================
 
w_cl = 23;
KP = (2*w_cl*zeta_cl*time_const - 1)/Km;
KI = (time_const/Km)*w_cl^2;
 
fprintf('\n--- Nominal Design (w_cl = %d) ---\n', w_cl);
fprintf('KP = %.4f\n', KP);
fprintf('KI = %.4f\n', KI);
 
Gc = KP + KI/s;
Gm = Km/(time_const*s + 1);
L  = Gc*Gm;
T  = feedback(L,1);
 
[GM, PM, ~, ~] = margin(L);
GM_dB = 20*log10(GM);
fprintf('PM = %.2f deg  (%s)\n', PM,  tf_str(PM  >= 45));
fprintf('GM = %.2f dB   (%s)\n', GM_dB, tf_str(GM_dB >= 10));
 
figure; margin(L); grid on; title('Open-loop Margin (nominal w_{cl}=23)');
 
%% Disturbance Rejection |P(jw)|
P = -1/(1+L);

w = logspace(-1,3,500);     % rad/sec
f = w/(2*pi);              % Hz

[mag,~] = bode(P,w);
mag = squeeze(mag);

figure;
semilogx(f,mag,'LineWidth',1.5);
hold on;
yline(0.1,'r--','Spec: 0.1','LineWidth',1.2);

grid on;
xlabel('Frequency [Hz]');
ylabel('|P(j\omega)|');
title('Disturbance Rejection |e/\omega_b|');
%% ==========================================================
%% 4. ANALYSIS MODEL (Simulink)
%% ==========================================================
 
assignin('base','KP', KP);
assignin('base','KI', KI);

assignin('base','DZ_lower', -DZ);
assignin('base','DZ_upper',  DZ);

assignin('base', 'SAT_lower', -SAT);
assignin('base', 'SAT_upper', SAT);
 
t    = (0:Ts:5)';
simin = [t, ones(size(t))];
assignin('base','simin', simin);
 
simout = sim('Analysis_model', 'StopTime', '5');

if exist('simout','var')
    try
        figure;
        plot(simout.simout.Time, simout.simout1.Data, 'LineWidth',1.5);
        grid on;
        xlabel('Time [sec]'); ylabel('Output');
        title('Analysis Model Response');
    catch e
        disp(e.message);
        disp(fieldnames(simout));  % shows you the actual variable names
    end
end
 

