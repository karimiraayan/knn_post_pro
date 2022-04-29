fileID = fopen('output.bin');
A = fread(fileID,[25 65536],'uint16');
fclose(fileID);
c=1;
B=zeros(25,65536);
for i=1:50 
    if mod(i,2)==0
        B(c,:)=A(i,:);
        c=c+1;
    end
end

B=B';