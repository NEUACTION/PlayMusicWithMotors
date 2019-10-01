clear
clc
warning('off');
%读取音频
[y,Fs] = audioread('f.mp3');
%读取长度
L = length(y(:,1));
%取每播放段长度为1000，实际时间为 总时间长度  * t / L
%以双簧管为例，每段播放时长为20ms，在此期间内，所有音源的频率不变
t =   10000;
dead = 0.001;
%十二平均律的中心频率，三个八度,从低音部开始
%利兹与青鸟
standardChordNote = [123.47 130.81 138.59 146.83 155.56 164.81 174.61 185.00 196 207.65 220 233.08 246.94...
    261.6 277.18 293.66 311.13 329.63 349.23 369.99 392 415.3 440 466 493.88...
    523.25 554.37 587.33 622.25 659.26 698.46 739.99 783.99 830.61 880 932.33 987.77...
    1046.5 1108.73 1174.66 1244.51 1318.51 1396.91 1479.98 1567.98 1661.22 1760 1864.66 1975.53];


% standardChordNote = [246.94...
%     261.6 277.18 293.66 311.13 329.63 349.23 369.99 392 415.3 440 466 493.88...
%     ];
for i = 1 : length(standardChordNote) - 1
    rangeNode(i) =  (standardChordNote(i) + standardChordNote(i+1)) / 2;
end
soundNum = 2;
istart = rangeNode(1) * t /Fs;
iend   = rangeNode(end) * t / Fs;

% spec = linspace(fstart,fend,soundNum-1);
figure;

num = 1 : t : L;
for j = 1 : length(num) - 1
    %片段
    axis([0 rangeNode(end) 0 0.1]);
    hold on;
    for i = 1 : length(rangeNode)
        a1 = floor(rangeNode(i) * t / Fs + 1);
        plot([floor(a1 *  Fs / t) floor(a1 *  Fs / t)],[0 0.1],'-k')
    end
    
    piece = y(num(j)+1:num(j+1),1);
    f = Fs*(0:(t/2))/t;
    %做傅里叶变换
    Y = fft(piece);
    %频段提取处理
    P2 = abs(Y/t);
    %由于FFT后，得到对称的频谱，取其中一半进行处理
    P1 = P2(1:t/2+1);
    % for i = 1 : length(P1)
    %     if P1(i) < 
    % end
  
  
  
%     P1(floor(fstart / Fs * t) : floor(fend / Fs * t-1)) = 2*P1(floor(fstart / Fs * t):floor(fend / Fs * t-1));
    %找到本剪切段内幅值最大处
%     [pks locs] = findpeaks(P1(floor(fstart / Fs * t) : floor(spec(1) / Fs * t-1)),'minpeakheight',0.0001); 
   pks = 0;locs = 0;recordFlag = 0;
    record = P1(floor(rangeNode(1) * t / Fs) + 1); 
    Tag = 0;
    for i = 1 : length(rangeNode) - 1
    try    
%       [pk loc] = findpeaks(P1(floor(rangeNode(i) * t / Fs + 1) : floor(rangeNode(i+1) * t / Fs)),'minpeakheight',0.001);    
       
       [pks(i) mI] = max(P1(floor(rangeNode(i) * t / Fs + 1) : floor(rangeNode(i+1) * t / Fs)+1));    

    catch ME
        pks(i) = max(P1(floor(rangeNode(i) * t / Fs + 1) : floor(rangeNode(i+1) * t / Fs)));
        recordFlag = 1;
        continue;
    end
%     if   isempty(pk) %没有峰值的情况，可能有：1。
%         len = length(P1(floor(rangeNode(i) * t / Fs + 1) : floor(rangeNode(i+1) * t / Fs)));
%         [mV mI] = max(P1(floor(rangeNode(i) * t / Fs + 1) : floor(rangeNode(i+1) * t / Fs)));
        if pks(i) < dead
            pks(i) = 0;%幅值太小
        else
            if mI == 1  %处理边缘问题
                if  pks(i) > record 
                     pks(i) = pks(i);
%                   disp('yes')

                    else if pks(i) <= record
                        if record >= pks(i) && pks(i) ~= 0 && i > 1
                            pks(i-1) = record; %设置上一次的峰值
                            pks(i) = 0;
                        end
                             
                        end
                end
                
            
%             else if mI == len
%                record = mV;
%             pks(i) = 0;
%                 end
            end
        end
%     else
%         [pks(i) locs(i)] = max(pk);  
%         recordFlag = 1;
%           pks(i) = 
%     end
%     
   plot(pks(i))
   record = pks(i);%P1(floor(rangeNode(i+1) * t / Fs)); 
   end
    %筛选出幅值前三处
    hold on;
    [maxVal maxInd] = max(pks);
    rec = maxInd;
    i = 1;
    recVal = 0;
    flag = 0;
    while i <= soundNum
  
        [maxVal maxInd] = max(pks);
        %记录此片段频率
        for q = 1 : length(rec)
            if abs(maxInd - rec(q)) < 2 && abs(rec(q) - maxInd) > 0
                pks(maxInd) = 0;
                flag = 0;
                break;
            else
                flag = 1;
            end
        end
            
        if flag == 1
            collectFreq(j,i) = standardChordNote((maxInd + 1));
            rec = [rec maxInd];
            plot(collectFreq(j,i),pks(maxInd),'*r')
            pks(maxInd) = 0;
            i = i + 1;
            flag = 0;
        end
    end  
%     if isempty(locs)
%         for i = 1 : soundNum
%         %记录此片段频率
%         collectFreq(j,i) = 0;
%         end
%     else
      stem(f,P1,'*b');
%     plot(f,P1,'b')
     
    title('Single-Sided Amplitude Spectrum of X(t)')
    xlabel('f (Hz)');
%     collectFreq(j,:) = sort(collectFreq(j,:),'descend');
    pause(0.01)
    hold off
    plot(0,0)
    
end
% pks = sort(pks,'descend');
% pks = pks(1:3);
% 
% collectFreq(1) = Fs * (find(P1,pks(1)))/t
% collectFreq(2) = Fs * (find(P1,pks(2)))/t
% collectFreq(3) = Fs * (find(P1,pks(3)))/t


temp = 0;
output1 = [];
for i = 1 : soundNum
    for j = 1 : length(collectFreq(:,i))
        for tick = 1 : t
          signal(tick,j) = sin(2*pi*collectFreq(j,i) *tick/Fs);
        end
    end
    if i == 1
        output = reshape(signal,[t*length(collectFreq(:,i)) 1]);
    else
        output = [output reshape(signal,[t*length(collectFreq(:,i)) 1])];
    end
end

sound((output(:,1)),44100)
sound((output(:,2)),44100)
 
% sound((output(:,3)),44100);
% sound((output(:,4)),44100);
% sound((output(:,5)),44100);
% sound((output(:,6)),44100);
% sound((output(:,7)),44100);
% sound((output(:,8)),44100);
% sound((output(:,9)),44100);
% sound((output(:,10)),44100);
% sound((output(:,11)),44100);
% sound((output(:,12)),44100);
% sound((output(:,13)),44100);
% sound((output(:,14)),44100);
% sound((output(:,15)),44100);
% plot(f(),P2(t/2+1:end));


