close all;
clear all;

NTRAZAS = 100;
NDEC = 16;

fid = fopen('traces.bin');
datos = fread(fid,(1024*1024-16)*NTRAZAS,'uint8');
fclose(fid);

datos = datos(1:end)*-1 + 255;

figure
hold all

%set(gca,'xtick',[0:31.25:1024*1024-16])
%set(gca,'xgrid','on')


%delayList = [0 6];

for i=1:NTRAZAS
	traza = datos((i-1)*(1024*1024-16)+1:(i)*(1024*1024-16));
	
	%Magic filtering
	%trazaF = fft(traza);
	%trazaF(1400:1800) = 0;
	%trazaF(16*1024-1800:16*1024-1400) = 0;
	
	%traza = real(ifft(trazaF));
	
	trazas{i} = traza;
	trazas{i} = trazas{i} - mean(trazas{i});
	
	
	plot(trazas{i});
	
	if i~= 1
	
		[trazas{i}, trazas{1}] = alignsignals(trazas{i}, trazas{1}, 1000, 'truncate');


		% dAct = delayList(i);
		% if dAct > 0
			% trazas{i} = [trazas{i}(dAct:end);zeros(dAct+1,1)];
		% else
			% trazas{i} = [zeros(-dAct,1);trazas{i}(1:end+dAct)];
		% end
	end
end

%legend('1','2');

figure
hold all;

fidSalida = fopen('traces_proc.bin','w');
fwrite(fidSalida, NTRAZAS, 'uint32');
fwrite(fidSalida, length(trazas{i})/NDEC, 'uint32');

for i=1:NTRAZAS
	%trazas{i} = filter([ones(1,NDEC)], [1], trazas{i});
	trazas{i} = decimate(trazas{i},NDEC,'fir');	
	
	%[C21, lag21] = xcorr(trazas{i}, trazas{1});
	%C21 = C21/max(C21);
	%[M21,I21] = max(C21);
	%t21 = lag21(I21);
	%fprintf('%d\t%f\n', i, t21);
	
	plot(trazas{i});
	fwrite(fidSalida,trazas{i},'double');
end

fclose(fidSalida);