sed -i.bak ' 
s/DOUBLE/dvariable/g; s/VECTOR/dvar_vector/g; s/MATRIX/dvar_matrix/g; s/D3_ARRAY/dvar3_array/g
/include/!s/<.*>//
s/template//' $1
#
# caution this wrecks #include statements, too
# s/<.*>//

