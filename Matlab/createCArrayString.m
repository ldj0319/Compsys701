function [string] = createCArrayString(array)
    ch1 = transpose(array(:,:,1));
    ch1 = ch1(:);
    
    ch2 = transpose(array(:,:,2));
    ch2 = ch2(:);
    
    ch3 = transpose(array(:,:,3));
    ch3 = ch3(:);
    
    arr = cat(1, ch1, ch2, ch3);
    
    string = '{';
    for i = 1:length(arr)
       string = strcat(string, num2str(arr(i)), ', ');
    end
    string = string(1:end-1);
    string = strcat(string, '}');
end
