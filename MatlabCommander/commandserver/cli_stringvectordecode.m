function stringlist=cli_stringvectordecode(paramValue)
% Creates a string cell array from the string-vector parameter value
% (which is a comma-separated string).
% More recent Matlab versions can return the value simply by using the
% strsplit function.

stringcellarray=regexp(paramValue,'([^,]*)','tokens')
stringlist=cat(2,stringcellarray{:})
