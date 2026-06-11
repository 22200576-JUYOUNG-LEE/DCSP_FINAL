%subject:   DCSP
%title:     Modeling
clc; clear all; close all;

search_dir      = '..\..\data\BodeMag\';
search_freqs    = 0.02:0.10:0.98; 

% lsqcurvefit 옵션 (출력 끄고, 반복 제한)
options = optimoptions('lsqcurvefit', ...
    'Display', 'off', ...          % 메시지 숨김
    'MaxIterations', 200, ...      % 반복 상한
    'MaxFunctionEvaluations', 5000); % 함수 평가 상한

%% Curve fitting
save_time      = [];
save_Vin       = [];
save_Wgyro     = [];

save_freq      = [];
save_Gs_mag    = [];
save_Gs_phase  = [];
save_Bias_in   = [];
save_Bias_out  = [];
save_Values    = [];

for freq = search_freqs
    serch_file = sprintf('%sBodeMag_sin_freq%.2f.out', search_dir, freq);

    if ~isfile(serch_file), fprintf("\n%.2f file not exist",freq); continue; end
    data = readmatrix(serch_file, 'FileType', 'text', 'NumHeaderLines', 1);

    data_time = data(:, 1);
    data_Vcmd = data(:, 2); 
    data_Wgyro = data(:, 5); 

    save_time{end +1} = data_time;
    save_Vin{end +1} = data_Vcmd;
    save_Wgyro{end +1} = data_Wgyro;
    
    % System least square curve fit
    lsq_timeDely_ZOH = 0.05;
    lsq_offset  = 0.0;
    lsq_phase   = 0.0;
    lsq_mag_in  = 0.7;
    lsq_mag_out = 780;
    SysResp = @(x, ft) x(1)*sin(2*pi*ft + x(2) + lsq_timeDely_ZOH) + x(3);

    % lsqcurvefit 
    Xin  = lsqcurvefit(SysResp, [lsq_mag_in; lsq_phase; lsq_offset], freq*data_time, data_Vcmd,  [], [], options);
    Xout = lsqcurvefit(SysResp, [lsq_mag_out; lsq_phase; lsq_offset], freq*data_time, data_Wgyro, [], [], options);

    % estimate parameters
    eMag_in  = Xin(1);
    ePhs_in  = Xin(2);
    eBias_in = Xin(3);

    eMag_out  = Xout(1);
    ePhs_out  = Xout(2);
    eBias_out = Xout(3);
    
    % find Gs_magnitude and Gs_phase
    save_Gs_mag(end +1) = eMag_out / eMag_in;
    save_Gs_phase(end+1) = asin( sin( ePhs_out - ePhs_in ) );

    %보정
    if (save_Gs_mag(end) < 0.0)
        save_Gs_mag(end) = abs(save_Gs_mag(end)) ;
        save_Gs_phase(end) = save_Gs_phase(end) - pi;
    end

    % Bias
    save_Bias_out(end+1)= eBias_out;
    save_Bias_in(end+1)  = eBias_in;

    % frequency 저장
    save_freq(end+1) = freq;

    % save Values
    save_Values(end+1,1) = save_freq(end);
    save_Values(end,2)   = save_Gs_mag(end);
    save_Values(end,3)   = save_Gs_phase(end);
    save_Values(end,4)   = save_Bias_in(end);
    save_Values(end,5)   = save_Bias_out(end);

end

%% invfreqs
tblOmega    = save_freq .* 2*pi;                        % [rad/s]
tblfreqResp = save_Gs_mag .* exp(1j * save_Gs_phase);   % H(jw)
[num,den] = invfreqs(tblfreqResp, tblOmega, 0, 1);

%% get each factor in fundamental second sys
Gs = tf(num, den)
% factor = MODEL_factors(Gs);
% fprintf("gain_k = %f",factor(1));
% fprintf("Wn = %f",factor(2));
% fprintf("zeta = %f",factor(3));

for k = 1:10
    ts = timeseries(save_Vin{k}, save_time{k});
    ts_fixed = setuniformtime(ts, 'StartTime', save_time{k}(1), 'EndTime', save_time{k}(end));
    time_uniform = ts_fixed.Time;

    y = lsim(Gs,save_Vin{k} - eBias_in ,time_uniform) + eBias_out;
    

    figure();
    plot(save_time{k},save_Wgyro{k});hold on;
    plot(save_time{k}, y, 'LineWidth',2);
    hold off;
    title('measured vs lsim %.2f',save_freq(k));
    xlabel('Time [sec]');
    ylabel('Voltage [V]');
    legend('measured', 'lsim');
end

[mag_bd, phase_bd, freq_bd] = bode(Gs, {min(tblOmega),max(tblOmega)});

mag_bd = squeeze(mag_bd);
phase_bd = squeeze(phase_bd);
freq_bd = squeeze(freq_bd);

%% Plot magnitude
figure(1);
semilogx(freq_bd,20*log10(mag_bd));
hold on;
semilogx(tblOmega, 20*log10(save_Gs_mag), 'rx');
title('Magnitude');
ylabel('Magnitude [dB]');
xlabel('\omega [rad/s]');
hold off;
grid on;

%% Plot phase (deg)
figure(2);
semilogx(freq_bd,phase_bd);
hold on;
semilogx(tblOmega, save_Gs_phase .* 180/pi, 'rx');
title('Phase');
ylabel('Phase [deg]');
xlabel('\omega [rad/s]');
hold off;
grid on;