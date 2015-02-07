//$Id: createParam.cpp 2920 2012-01-18 02:51:43Z eunjung $
#include "par_t_reg.h"
#include "par_t_nn.h"


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* createParam(const adstring& filename) {
	par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE> *param = 0;

	cifstream cifs(filename + ".par");
	if (!cifs) {
		cerr << "Error: Cannot open file " << filename << ".par\n";
		return 0;
	}
	adstring signature = cifs.signature();
	cifs.close();

        cout << "from createParam: " << endl;
	//cout << "Info: " << filename << ".par file version = \"" << signature << "\"" << endl;
	if (signature == "#v20r\0") {
		param = new par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(filename);
	} else  if (signature == "#v25n\0") {
		param = new par_t_nn<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(filename);
	} else {
		cerr << "Error: incorrect file signature \"" << signature << "\" in " << filename << ".par\n";
	}
        cout << "from createParam: " << endl;

	return param;
}

extern template class par_t_reg<d3_array,dmatrix,dvector,double>;
extern template class par_t_nn<d3_array,dmatrix,dvector,double>;
extern template class par_t<d3_array,dmatrix,dvector,double>;
template par_t<d3_array,dmatrix,dvector,double>* createParam<d3_array,dmatrix,dvector,double>(const adstring& filename);

extern template class par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
extern template class par_t_nn<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
extern template class par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
template par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* createParam<dvar3_array,dvar_matrix,dvar_vector,dvariable>(const adstring& filename);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::createInstance(const adstring &filename) {
        if (instance == 0) {
                instance = createParam<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(filename);
        }
        return instance;
}
template par_t<d3_array,dmatrix,dvector,double>* par_t<d3_array,dmatrix,dvector,double>::createInstance(const adstring &filename);
template par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>* par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::createInstance(const adstring &filename);
