% subject: Digital control and signal Processing
% title: Linearization
% author: Juyoung Lee
clc;
clear all;
close all;

%--------------------------------------------------------------------------
% Import Static Property 
%--------------------------------------------------------------------------
for idx = 1 : 11

    current_file = sprintf('../data/Static/Static_iter%d.out', idx);

    if isfile(current_file)
        Dataset = readmatrix(current_file, 'NumHeaderLines', 1, 'FileType', 'text');
        Motor_Volt(:,idx) = Dataset(:,1);
        Rate_Gyro (:,idx) = Dataset(:,2);
    else
        fprintf('Cannot find a file: %s\n', current_file);  
    end
end

Mean_Rate_Gyro  = mean(Rate_Gyro, 2, 'omitnan');
Mean_Motor_Volt = mean(Motor_Volt, 2,'omitnan');

%--------------------------------------------------------------------------
% Static Characteristics of your gimbal motor 
%-------------------------------------------------------------------------- 
Wg = Mean_Rate_Gyro * 180/ pi ;           % mean of rate gyro outputs [deg/s] 
Vm = Mean_Motor_Volt ;           % mean of motor driving voltage [V]  

%-------------------------------------------------------------------------- 
% Linearization Function Design  
%-------------------------------------------------------------------------- 

% Motor Angular Speed  
MAX_RATE = 1300; % [deg/s]  
MIN_RATE = 50; % [deg/s]  

% Extract Data Points for CW and CCW Directions  
[~,idxN1]= min(abs(Wg - (-MAX_RATE))) ; % CCW direction   
[~,idxN2]= min(abs(Wg - (-MIN_RATE))) ; 
[~,idxP1]= min(abs(Wg - MIN_RATE))    ; % CW  direction
[~,idxP2]= min(abs(Wg - MAX_RATE))    ;

%-------------------------------------------------------------------------- 
% Ployfitting for Getting Coefficient  
%-------------------------------------------------------------------------- 
CoefP = polyfit(Wg(idxP1:idxP2), Vm(idxP1:idxP2), 4);
CoefN = polyfit(Wg(idxN1:idxN2), Vm(idxN1:idxN2), 4);

%-------------------------------------------------------------------------- 
% Validity Check of Linearization Algorithm  
%-------------------------------------------------------------------------- 
bufWc = -1300:1:1300 ; % control command of a stabilization loop   
N = length(bufWc) ;  

Wc_DZ  = 50.0 ;     % desired dead-zone  [deg/s]  
Wc_SAT = 1000.0 ;   % desired saturation [deg/s]   

for idx = 1 : N
    Wc = bufWc(idx) ; % control command  
    %---------------------------------------------------------------------- 
    % LINEARIZATION FUNCTION  
    %---------------------------------------------------------------------- 
 
    if( Wc >= Wc_DZ ) % CW  
 
        Wc = min( Wc, Wc_SAT ) ;  
        H = [ Wc^4 ; Wc^3 ; Wc^2 ; Wc ; 1 ] ;  
 
        Vcmd(idx) = min( CoefP*H, Wc_SAT) ;  
 
    elseif( Wc <= -Wc_DZ ) % CCW  
 
        Wc = max( Wc, -Wc_SAT ) ;  
        H = [ Wc^4 ; Wc^3 ; Wc^2 ; Wc ; 1 ] ;  
 
        Vcmd(idx) = max( CoefN*H, -Wc_SAT) ;  
         
    else % Deadzone  
 
        Vcmd(idx) = 2.5 ; % stop voltage is issued to    the motor    
 
    end
    
    %---------------------------------------------------------------------- 
    % EXPECTED LINEARIZATION RESULT   
    %---------------------------------------------------------------------- 
    % Wcmd --> Linearization Function --> Vm --> Motor --> Wout  
    %---------------------------------------------------------------------- 
 
    Wout(idx) = interp1( Vm, Wg, Vcmd(idx) ) ;  

end

%-------------------------------------------------------------------------- 
% Plot Results   
%-------------------------------------------------------------------------- 
 
figure(1), plot(Wg, Vm, 'b', bufWc, Vcmd, 'r', 'LineWidth',2),  
title('Linearization Function Design: V_{m} = f(\omega_{c})', 'FontSize',13) 
legend({'static characteristics: V_{m} = g(\omega_{out})',...  
        'linearization function: V_{m} = f^{-1}(\omega_{c})'}, 'FontSize',12)  
grid on, box on, xlabel('Wc [deg/s]', 'FontSize',12), ylabel('Vcmd [V]', 'FontSize',12) 
set(gca, 'FontSize',12) 
 
figure(2), hold on;
plot(bufWc, Wout, 'LineWidth',2),  
plot(bufWc, bufWc, 'LineWidth',1.5);
title('Expected Linearization Result', 'FontSize',13)  
legend({'\omega_{c} --> f^{-1}(\omega_{c}) = V_{m} --> motor f(\cdot) --> \omega_{out}', 'Linear Line'}, ...
    'FontSize',12)  
grid on, box on, xlabel('Wc [deg/s]', 'FontSize',12), ylabel('Wout [deg/s]', 'FontSize',12) 
set(gca, 'FontSize',12) 

% =========================================================
% Save Data (..\data\LINEAR_COEF.csv)
% =========================================================
data_matrix = [CoefP; CoefN];
writematrix(data_matrix, '..\data\Linearization_coef.csv'); 