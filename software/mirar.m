close all;
clear all;

fid = fopen('traces_sin_caps.bin');
datos = fread(fid,'uint8');
fclose(fid);

datos = datos(1:end)*-1 + 255;

figure
plot(datos)

figure
hold all

set(gca,'xtick',[0:31.25:16*1024])
set(gca,'xgrid','on')
trazaPromedio = zeros(16*1024,1);

for i=0:9
	traza = datos(i*16*1024+1:(i+1)*16*1024);
	trazas{i+1} = traza;
	
	if i~= 0
		[C21, lag21] = xcorr(trazas{i+1}, trazas{i});
		C21 = C21/max(C21);
		[M21,I21] = max(C21);
		t21 = lag21(I21)

		if t21 > 0
			trazas{i+1} = trazas{i+1}(t21+1:end);
		else
			trazas{i+1} = [zeros(t21-1,1);trazas{i+1}];
		end
	end
	
	%traza = traza(7778:9539);
	%trazaCentrada = traza - mean(traza);
	%trazaPromedio = trazaPromedio + trazas{i+1}-mean(trazas{i+1});
	plot(traza);
end

%Verificar Xcorr max en instante 0
a=0
figure
hold all;

for i=1:9
	[C21, lag21] = xcorr(trazas{i+1}, trazas{i});
	C21 = C21/max(C21);
	[M21,I21] = max(C21);
	t21 = lag21(I21)
	
	plot(trazas{i+1});
end


%trazaPromedio = trazaPromedio * 0.1;




