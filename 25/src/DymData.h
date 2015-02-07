//$Id: DymData.h 2756 2011-01-04 20:42:23Z jsibert $
/** \file DymData.h
Class definiations for handling SEAPODYM data structures.
\ingroup seapodym
*/
#ifndef __DymData_h__
#define __DymData_h__

#include <vector>
#include <string>
using std::vector;
using std::string;

#include <fvar.hpp>

/**
 * DymData
 */
class DymData
{
public:
static const int DATA_BYTE_SIZE = 4;

public:
/**
 * Default Constructor.
 */
DymData() {;}
/**
 * Default Destructor.
 */
virtual ~DymData() {;}

public:
void read();
void write();
void print();

/**
 * Simple static function to just get the data matrix for area and date.
 */
static void getData(const adstring& file, 
                    const int year, 
                    const int month, 
                    const double swlongitude,
                    const int dx,
                    const double swlatitude,
                    const int dy,
                    dmatrix& data);

private:
/** 
 * @defgroup DymFormat Dym Version 2.0 Data Structure
 * Created in October 2004 to accoumodate mixed spatial resolution and a quicker access to the minimum and mizimum values of the DATA matrix.  Depending on the usage, the NLEVEL value and ZLEVEL array can be interpreted in various ways: a a depth variation or as time variation for examples.
 * Note: all data sored with file is 4 bytes long(32-bits). Bytes use little-endian ordering, they need to be swapped when reading from Java.
\ingroup seapodym
 * @{
 */
char	 _format[4];	/**< number for identifying the function used for grid stretching. 0 indicates no stretching. */
int	 _func;		/**< number for identifying the function used for grid stretching. 0 indicates no stretching. */
double	 _min_value;	/**< minimum value of the DATA field. */
double	 _max_value;	/**< maximum value of the DATA field. */
int	 _nlong;	/**< maximum records on the west->east axis. */
int	 _nlat;		/**< maximum records on the north->south axis. */
int	 _nlevel;	/**< maximum records on the depth axis (or time axis). */
double	 _start_date;	/**< the start date of the record. */
double	 _end_date;	/**< the end date of the record. */
dmatrix	 _xlon;		/**< 2D vector with all the longitude values of the grid points. */
dmatrix	 _ylat;		/**< 2D vector with all the latitude values of the grid points. */
double	 _zlevel;	/**< grid, depth (surface -> bottom) or time direction (START_DATE -> END_DATE). Maps matrix corrdinates to real values. */
imatrix	 _msksp;	/**< for the mask, we will ad additional value since we are taking into account several (3 in the final version) vertical layers.  
                             Logically we should have:
                             - 0: land
                             - 1: depth <= first layer boundary (200 m currently). For the moment this layer should be considered as land becuase the ESSIC model starts working the this depth limit.
                             - 2: depth <= second layer boundary (500 m: currently). 
                             - 3: depth <= last - 1 layer boundary (500 m: this will be for the next simulation with 3 layers).  This is important for calculating the vertical migration of forage components and the time they spend in the different. 
*/
d3_array _data;		/**< the 3D matrix data, this matrix is composed of NLEVEL overlapped 2D matrix of size NLONG*NLAT. Each 2D matrix in made of NLAT lines of NLONG values (west->east) that start from the northern values down to the sourthern ones.*/
/** @} */


/*
	friend class fishery_record;
	
private:
   	DMATRIX mat2d_NoBorder;

	//Inna: fishery file variables
	IMATRIX position_file, nrecords;
	IVECTOR nbsp_file, rec_count;
	DVECTOR startdate_fishery, enddate_fishery;

	i3_array position, numrec;
	//fishery_record* frec;

	int all_rec;
	d5_array frq;
	bool fisheries_data_read;

public:
    
	vector<string> dymFileFPred ;
	vector<string> dymFileSpPred ;
	vector<string> dymFileSpCobs ;
	vector<string> dymFileSpLF ;
	vector<string> dymFileSumSpLF ;
	vector<string> dymFileSpCorr ;
	string fileS ;
	string fileF ;


public:
	void rbin_headpar(string file_in, int &nlong, int &nlat, int &nlevel);

	void rtxt_headpar(string file_in, int &nlong, int &nlat, int &nlevel);

	void rwbin_minmax(string file_io, double minvalstep, double maxvalstep);//Gael Nov04

	void rtxt_mat2d(string file_in, DMATRIX& mat2d, int &nlong, int &nlat);
	
	void rbin_header(string file_in, string &idformat, int &idfunc, double &minval, double &maxval,  
					int nlong, int nlat, int nlevel, double &startdate, double &enddate,
					DMATRIX& xlon, DMATRIX& ylat, DVECTOR& zlevel, IMATRIX& msksp) ; 
	
	void wbin_header(string file_out, string &idformat, int &idfunc, double &minval, double &maxval, 
					int nlong, int nlat, int nlevel, double &startdate, double &enddate,
					const DMATRIX& xlon, const DMATRIX& ylat, const DVECTOR& zlevel, const IMATRIX& msksp) ;	
		
	void rtxt_header(string file_in, int nlong, int nlat, int nlevel,
					double &startdate, double &enddate,
					DVECTOR& xlon, DVECTOR& ylat, DVECTOR& zlevel, IMATRIX& msksp) ;

	void rbin_mat2d(string file_out, PMap& map, DMATRIX& mat2d, int nlat, int nlong, int nbytetoskip) ;
	
	void rbin_input2d(string file_in, DMATRIX& mat2d,
					int nbi, int nbj, int nbytetoskip) ;

	void wbin_mat2d(string file_out, const DMATRIX& mat2d,
					int nlat, int nlong, bool FILEMODE);
	
	void wbin_transpomat2d(string file_out, const DMATRIX& mat2d,
							int nlong, int nlat, bool FILEMODE);
	
	void wtxt_header(string file_out,int nlong, int nlat, int nlevel,
					double &startdate, double &enddate,
					const DVECTOR& xlon, const DVECTOR& ylat, const DVECTOR& zlevel, const IMATRIX& msksp) ;
	
	void wtxt_mat2d(string file_out, const DMATRIX& mat2d, int nlat, int nlong, bool FILEMODE);
	
	void rtxt_col_lonlat(string file_in, DMATRIX& mat2d,int nlong, int nlat,
					DVECTOR& xlon, DVECTOR& ylat, int nbvar, int var);
	
	void wbin_fishery(string file_in, string file_out, int nbvar);
	
	void rbin_fishery(string file_in, DMATRIX& mat2d, CParam& param, int nbvar, int nvar, int yyyy, int mm);
	
	void InitSepodymFileDym(CParam& param, CMatrices& mat, int nb_mo, DVECTOR& zlevel, const IMATRIX& msksp);
	
	void SaveSepodymFileDym(CParam& param, PMap& map, CMatrices& mat);

	void SaveDymFile(PMap& map, CMatrices& mat, string file, const dmatrix& data, const int nlon, const int nlat);
	
	void InitSepodymFileTxt(CParam& param);
	
	void SaveSepodymFileTxt (CParam& param, CMatrices& mat, PMap& map,
					double sumP, DVECTOR& sumF, DVECTOR& sumFprime, DVECTOR& sumF_area_pred, DVECTOR& sumF_required_by_sp, 
					DVECTOR& mean_omega_sp, int mois2, int yr2, int t_total, 
					int ntq, int qtr1, int qtr2, int nbi, int nbj);
	//Inna 05Jul05: reading GMB fisheries files format and destroying variables after all
	void rbin_fishery_header(CParam& param);
//	void rbin_fishery(CParam& param, D3_ARRAY& effort, D4_ARRAY& catch_obs, float date, bool& fishing);
//	void rbin_fishery(CParam& param, D3_ARRAY& effort, float date, const int sp);
//	void rbin_fishery(CParam& param, PMap& map, dmatrix& effort, float date, const int f);
	//-----------------------------------------------------------------------------------
	void read_fishery_data(CParam& param, const PMap& map);
	void write_fishery_data(CParam& param, const PMap& map, CMatrices& mat, const int sp, const int year, const int month, bool FILEMODE);
	void delete_fisheries_rec(void);
	void get_effort(CParam& param, dmatrix& effort, const int f, int y, const int m);
	void get_fishery_data(CParam& param, D3_ARRAY& effort, D4_ARRAY& catch_obs, int y, const int m);
	void get_average_effort(CParam& param, D3_ARRAY& effort, D4_ARRAY& catch_obs, const int nby, const int m);

	int get_numrec(const int f, const int y, const int m){ return numrec(f,y,m); }

	void read_lf_WCPO(CParam& param, string filename, const float startdate, const float enddate, const int sp);
	void read_lf_EPO(CParam& param, string filename, const float startdate, const float enddate, const int sp);
	void read_frq_data(CParam& param, const float startdate, const float enddate, const int sp);
	void get_LF_qtr_data(CParam& param, d4_array LF_qtr_obs, int y, const int q);

	void write_frq_data(CParam& param, int sp, int year, int qtr, d3_array frq, bool FILEMODE);
	void read_pred_frq_data(CParam& param, string filename, const float startdate, const float enddate, const int sp);
*/
};
/*
class fishery_record
{
  public:
	fishery_record() {;}
	virtual ~fishery_record() {;}
  private:
    int i,j;
    double efrt;
    dvector ctch;

  public:
	int get_i(){return i;}
	int get_j(){return j;}
	double get_effort(void){return efrt;};
	dvector get_catch(void){return ctch;};

	void set_record(int ii, int jj, double ee, dvector cc){i = ii; j = jj; efrt=ee; ctch=cc;}
};
*/
#endif
