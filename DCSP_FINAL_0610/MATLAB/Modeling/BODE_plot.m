%% DC_PROJECT
% 3. Validation PLOT Wqyro
function BODE_plot(base_dir)
    Time = [];
    
    for idx = 0.3 :0.2: 1.3
        % 1. 동적 파일 이름 및 레전드 이름 생성
        filename = sprintf('%sBodeMag_sin_freq%.2f.out', base_dir, idx);
    
        if isfile(filename)
            % 2. Get Dataset
            Dataset = readmatrix(filename, 'NumHeaderLines', 1, 'FileType', 'text');

            if isempty(Time)
                Time = Dataset(1000:end,1); % 첫 유효 데이터에서 시간축 획득
            end

            Vcmd = Dataset(1000:end,2);
            Wgyro = Dataset(1000:end,4);

            Vcmd = normalize(Vcmd, 'range', [-8 8]);

            figure(); hold on;
            plot(Time, Vcmd, 'b-', 'LineWidth', 1.5, 'DisplayName', 'V_{cmd\_mean}');
            plot(Time, Wgyro, 'r-', 'LineWidth', 1.5, 'DisplayName', '\omega_{gyro\_mean}');


            %4. Labling
            xlabel('Time[sec]','FontWeight','bold');
            ylabel('normalized V_{cmd}[V] vs \omega_{gyro}[rad/sec]','FontWeight','bold');
            
            title(idx, 'Hz');
            legend('show', 'Location', 'best');
            hold off; grid on;



        else
            % fprintf('파일을 찾을 수 없습니다: %s\n', filename);  % 파일이 없을 경우 경고 메시지 출력 (디버깅 용도)
        end
    end

 
