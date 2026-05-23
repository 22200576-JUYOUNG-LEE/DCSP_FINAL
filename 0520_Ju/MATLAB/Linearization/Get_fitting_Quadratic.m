%% DC_PROJECT
% 1. PLOT DATASET
function [coeff_pos, coeff_neg, pos_start, neg_start] = Get_fitting_Quadratic(filename)

    data=readmatrix(filename, 'NumHeaderLines', 1, "FileType","text");

    Vc=data(:,1); %[V] 
    Wgyro=data(:,2); %[deg/sec]

    pos_start = 2.70; pos_end = 4.00;
    neg_start = 2.30; neg_end = 1.00;

    OperPoint_Vc = 2.74;
    [~, OperPoint_idx] = min(abs(Vc - OperPoint_Vc));
    OperPoint_Wgyro = Wgyro(OperPoint_idx);

    pos_region = (Vc >= pos_start) & (Vc <= pos_end); 
    neg_region = (Vc >= neg_end) & (Vc <= neg_start);


    Vc_pos=Vc(pos_region);
    Wgyro_pos=Wgyro(pos_region); 

    A_pos=[Vc_pos.^2, Vc_pos, ones(size(Vc_pos))];
    B_pos=Wgyro_pos; 

    coeff_pos = A_pos \ B_pos; 

    Vc_neg = Vc(neg_region);
    Wgyro_neg = Wgyro(neg_region);

    A_neg = [Vc_neg.^2, Vc_neg, ones(size(Vc_neg))];
    B_neg = Wgyro_neg;

coeff_neg = A_neg \ B_neg;

data_matrix = [OperPoint_Vc, OperPoint_Wgyro; coeff_pos, coeff_neg];

writematrix(data_matrix, '..\data\LIN_COEF.csv');

% % conclusion: 
% fprintf('Positive region: W = %.4f*V^2 + %.4f*V + %.4f\n', coeff_pos);
% fprintf('Negative region: W = %.4f*V^2 + %.4f*V + %.4f\n', coeff_neg);
% 

end