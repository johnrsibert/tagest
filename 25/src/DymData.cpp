//$Id: DymData.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "DymData.h"
#include <string>
using std::string;
using std::ifstream;

string getHeader(ifstream& ifs)
{
	char header[DymData::DATA_BYTE_SIZE + 1];
	ifs.read(&header[0], DymData::DATA_BYTE_SIZE); 
	header[DymData::DATA_BYTE_SIZE] = '\0';
	return string(header);
}
int getInteger(ifstream& ifs)
{
	int value = 0;
	ifs.read((char*)&value, DymData::DATA_BYTE_SIZE); 
	return value;
}
float getFloat(ifstream& ifs)
{
	float value = 0;
	ifs.read((char*)&value, DymData::DATA_BYTE_SIZE); 
	return value;
}
double getDouble(ifstream& ifs)
{
	return double(getFloat(ifs));
}
int computeTimeId(const int year, const int month)
{
	int timeId = year * 100;
	switch (month) {
	case 1:
		timeId += 4;
		break;
	case 2:
		timeId += 12;
		break;
	case 3:
		timeId += 21;
		break;
	case 4:
		timeId += 29;
		break;
	case 5:
		timeId += 38;
		break;
	case 6:
		timeId += 46;
		break;
	case 7:
		timeId += 54;
		break;
	case 8:
		timeId += 62;
		break;
	case 9:
		timeId += 71;
		break;
	case 10:
		timeId += 79;
		break;
	case 11:
		timeId += 88;
		break;
	case 12:
		timeId += 96;
		break;
	default:
		timeId = -1;
		break;
	}
	return timeId;
}
int readDYMv2File(const char* filename,
		  const int year,
		  const int month,
		  dmatrix& data)
{
        std::ifstream ifs(filename, ios::binary|ios::in);
        if (ifs.is_open() == false) {
                return -1;
        }

	const string id = getHeader(ifs);
	const int func = getInteger(ifs);
	const float min_value = getFloat(ifs);
	const float max_value = getDouble(ifs);
	const int nlong = getInteger(ifs);
	const int nlat = getInteger(ifs);
	const int nlevel = getInteger(ifs);
	const float start_date = getFloat(ifs);
	const float end_date = getFloat(ifs);
	for (int j = 0; j < nlat; j++) {
		for (int i = 0; i < nlong; i++) {
			const float xlon = getFloat(ifs);
		}
	}
	for (int j = 0; j < nlat; j++) {
		for (int i = 0; i < nlong; i++) {
			const float ylat = getFloat(ifs);
		}
	}
	const int targetTimeId = computeTimeId(year, month);
	int targetLevelIndex = -1;
	for (int level = 0; level < nlevel; level++) {
		const float zlevel = getFloat(ifs);
		if (int(zlevel * 100) == targetTimeId) {
			targetLevelIndex = level;
		}
	}
	for (int j = 0; j < nlat; j++) {
		for (int i = 0; i < nlong; i++) {
			const int msksp = getInteger(ifs);
		}
	}
	data.allocate(1, nlong + 1, 1, nlat + 1);
	for (int level = 0; level < nlevel; level++) {
		if (level == targetLevelIndex) {
			for (int j = 0; j < nlat; j++) {
				for (int i = 0; i < nlong; i++) {
					const double dataValue = getDouble(ifs);
					data(i + 1, j + 1) = dataValue;
				}
			}
		} else {
			for (int j = 0; j < nlat; j++) {
				for (int i = 0; i < nlong; i++) {
					const float dataValue = getFloat(ifs);
				}
			}
		}
	}
	ifs.close();
	return 0;
}
void DymData::getData(const adstring& file,
                      const int year,
                      const int month,
                      const double swlongitude,
                      const int dx,
                      const double swlatitude,
                      const int dy,
                      dmatrix& data)
{
}

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef DYM_OUTPUT_TYPE
	#define DYM_OUTPUT_TYPE float
#endif
#ifndef DYM_OUTPUT_MY_TYPE
	#define DYM_OUTPUT_MY_TYPE double
#endif

#ifndef DYM_INPUT_TYPE
	#define DYM_INPUT_TYPE float
#endif

fishery_record* frec;

/////////////////////////////////////////////////
////////////////////////// LECTURE BINAIRE//////////////////////
/////////////////////////////////////////////////
// lecture des parametres du fichier permettant de creer leas tableaux
void CReadWrite::rbin_headpar(string file_in, int &nlong, int &nlat, int &nlevel)
{
//cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
	ifstream litbin(file_in.c_str(), ios::binary|ios::in);
	if (!litbin)
	{
		cerr << "Error[" << __FILE__ << ':' << __LINE__ << "]: Unable to read file \"" << file_in << "\"\n";
		exit(1);
	}

 	const int nbytes= 4;

	// for new dym format, skip the first 4 parameters (Idformat, Idfunc, Minval, Maxval)
	const int nbytetoskip = 4 * nbytes;  // Patrick 21Oct2004 
	litbin.seekg(nbytetoskip, ios::beg); // Patrick 21Oct04 

	int bufin;
	litbin.read((char *)&bufin,nbytes);
	nlong = bufin;
 	litbin.read((char *)&bufin,nbytes);
	nlat = bufin;
	litbin.read((char *)&bufin,nbytes);
	nlevel = bufin;

	litbin.close();
}
// reading the min/maxval and update them if necessary
void CReadWrite::rwbin_minmax(string file_io, double minvalstep, double maxvalstep)//Gael Nov04
{
	fstream rwbin(file_io.c_str(), ios::binary|ios::in|ios::out);

	if (!rwbin) {
		cerr << "Error[" << __FILE__ << ':' << __LINE__ << "]: Unable to read-write file \"" << file_io << "\"\n";
		exit(1);
	}

 	const int nbytes= 4;
	//skip the first 2 parameters (Idformat, Idfunc)
	const int nbytetoskip = 2 * nbytes; 
	rwbin.seekg(nbytetoskip, ios::beg); 

 	const int sizeofDymOutputType = sizeof(DYM_OUTPUT_TYPE);
	DYM_OUTPUT_TYPE buffl;

	rwbin.read(( char *)&buffl, sizeofDymOutputType);	
	const double minval = buffl;

	if (minvalstep<minval) {

		rwbin.seekp(-sizeofDymOutputType, ios::cur);
		buffl = minvalstep;
		rwbin.write(( char *)&buffl, sizeofDymOutputType);
	}

	rwbin.read(( char *)&buffl, sizeofDymOutputType);	
	const double maxval = buffl;

	if (maxvalstep>maxval) {

		rwbin.seekp(-sizeofDymOutputType, ios::cur);
		buffl = maxvalstep;
		rwbin.write(( char *)&buffl, sizeofDymOutputType);
	}

	rwbin.close();
}

//---------------------------------------
// Lecture de l'entete
// nlong, nlat, nlevel, startdate, enddate
// val des longitudes, latitudes, dates
// mask
//---------------------------------------
void CReadWrite::rbin_header(string file_in, string &idformat, int &idfunc, double &minval, double &maxval,  
					int nlong, int nlat, int nlevel, double &startdate, double &enddate,
					DMATRIX& xlon, DMATRIX& ylat, DVECTOR& zlevel, IMATRIX& msksp)
{
//cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
	ifstream litbin(file_in.c_str(), ios::binary|ios::in);
	if (!litbin)
	{
		cerr << "Error[" << __FILE__ << ':' << __LINE__ << "]: Unable to read file \"" << file_in << "\"\n";
		exit(1);
	}
	const int nbytes= 4;
	
	int bufin;

 	const int sizeofDymInputType = sizeof(DYM_INPUT_TYPE);
	DYM_INPUT_TYPE buffl;

        char *bufch;
        int bufchlen = nbytes+1;
        bufch = new char [bufchlen];
        bufch[bufchlen-1] = '\0';

	  	
	/////////////////////////////////////////////
	// reading new dym format	: Patrick 21Oct04 
	litbin.read(( char *)bufch,nbytes);
	idformat = bufch;
	litbin.read(( char *)&bufin,nbytes);	
	idfunc = bufin;
	litbin.read(( char *)&buffl,sizeofDymInputType);	
	minval = buffl;
	litbin.read(( char *)&buffl,sizeofDymInputType);	
	maxval = buffl;
	/////////////////////////////: Patrick 21Oct04	
	litbin.read(( char *)&bufin,nbytes);
	nlong = bufin;
	litbin.read(( char *)&bufin,nbytes);
	nlat = bufin;
	litbin.read(( char *)&bufin,nbytes);
	nlevel = bufin;
	litbin.read(( char *)&buffl,sizeofDymInputType);
	startdate = buffl;
	litbin.read(( char *)&buffl,sizeofDymInputType);
	enddate = buffl;

	///////////////////////////////////////////
	// reading new dym format:  Patrick 21Oct04
	//---------------------------------------
	//xlon	valeurs des longitudes
	//---------------------------------------
	for (int i=0;i<nlat;i++)
	{
		for (int j=0;j<nlong;j++)
		{
			litbin.read(( char *)&buffl,sizeofDymInputType);
			xlon[i][j] = buffl;
		}
	}

	//---------------------------------------
	//ylat	valeurs des latitudes
	//---------------------------------------
	for (int i=0;i<nlat;i++)
	{
		for (int j=0;j<nlong;j++)
		{
			litbin.read(( char *)&buffl,sizeofDymInputType);
			ylat[i][j] = buffl;
		}
	}

	///////////////////////////: Patrick 21Oct04
	//---------------------------------------
	//zlevel: contient les valeurs des dates
	//---------------------------------------
	for (int k=0;k<nlevel;k++)
	{
		litbin.read(( char *)&buffl,sizeofDymInputType);
		zlevel[k] = buffl;
	}
	enddate = zlevel[nlevel-1];

	//---------------------------------------
	//msksp
	//---------------------------------------
	for (int i=0;i<nlat;i++)
	{
		for (int j=0;j<nlong;j++)
		{
			litbin.read(( char *)&bufin,nbytes);
			msksp[i][j] = bufin;
		}
	}
	delete [] bufch;

	litbin.close();
}
//////////////////////////////////////////////////////////////////////
void CReadWrite::rbin_mat2d(string file_in, PMap& map, DMATRIX& mat2d, int nlat, int nlong, int nbytetoskip)
{
	ifstream litbin(file_in.c_str(), ios::binary | ios::in);
	
	if (!litbin)
	{
		cerr << "Error[" << __FILE__ << ':' << __LINE__ << "]: Unable to read file \"" << file_in << "\"\n";
		exit(1);
	}

	//---------------------------------------
	// Reading the 2d matrix, written by seapodym_coupled
	//---------------------------------------
	litbin.seekg(nbytetoskip, ios::cur);

 	const int sizeofDymInputType = sizeof(DYM_OUTPUT_MY_TYPE);
	DYM_OUTPUT_MY_TYPE buf;

	for (int i=0;i<nlat;i++)
	{
		for (int j=0;j<nlong;j++)
		{
			litbin.read(( char *)&buf,sizeofDymInputType);
			if (map.carte(i,j))
				mat2d[i][j]= buf;
		}
	}

	litbin.close();
}

//////////////////////////////////////////////////////////////////////
void CReadWrite::rbin_input2d(string file_in, DMATRIX& mat2d,
							int nbi, int nbj, int nbytetoskip)
{
//cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
////////////// lecture fichier binaire
	ifstream litbin(file_in.c_str(), ios::binary | ios::in);
	
	if (!litbin)
	{
		cerr << "Error[" << __FILE__ << ':' << __LINE__ << "]: Unable to read file \"" << file_in << "\"\n";
		exit(1);
	}

	//---------------------------------------
	// Reading the 2d matrix
	//---------------------------------------
	litbin.seekg(nbytetoskip, ios::cur);


 	const int sizeofDymInputType = sizeof(DYM_INPUT_TYPE);
	DYM_INPUT_TYPE buf;

	for (int j=1;j<nbj-1;j++)
	{
		for (int i=1;i<nbi-1;i++)
		{
			litbin.read(( char *)&buf,sizeofDymInputType);
			mat2d[i][j]= buf;
		}
	}

	litbin.close();
}

/////////////////////////////////////////////////
////////////////////////// ECRITURE BINAIRE//////////////////////
/////////////////////////////////////////////////
void CReadWrite::wbin_header(string file_out, string &idformat, int &idfunc, double &minval, double &maxval, 
					int nlong, int nlat, int nlevel, double &startdate, double &enddate,
					const DMATRIX& xlon, const DMATRIX& ylat, const DVECTOR& zlevel, const IMATRIX& msksp)
{
	ofstream ecritbin(file_out.c_str(), ios::binary|ios::out);

	if (!ecritbin)
	{
		cerr << "Error[" << __FILE__ << ':' << __LINE__ << "]: Unable to write file \"" << file_out << "\"\n";
		exit(1);
	}

	const int nbytes= 4;
	int bufin;
 	const int sizeofDymOutputType = sizeof(DYM_OUTPUT_TYPE);
	DYM_OUTPUT_TYPE buf;

	//---------------------------------------
	// Write idformat, idfunc, minval, maxval	//Patrick 21Oct04
	//---------------------------------------
	//ecritbin.write(idformat.c_str(),nbytes);
	ecritbin << idformat.c_str();
	bufin = idfunc;
	ecritbin.write(( char *)&bufin,nbytes);
	buf = minval;
	ecritbin.write(( char *)&buf, sizeofDymOutputType);
	buf = maxval;
	ecritbin.write(( char *)&buf, sizeofDymOutputType);

	//---------------------------------------
	// Write nlong, nlat, nlevel, stardate, enddate
	//---------------------------------------
	bufin = nlong;
	ecritbin.write(( char *)&bufin,nbytes);
	bufin = nlat;
	ecritbin.write(( char *)&bufin,nbytes);
	bufin = nlevel;
	ecritbin.write(( char *)&bufin,nbytes);
	buf = startdate;
	ecritbin.write(( char *)&buf, sizeofDymOutputType);
	buf = enddate;
	ecritbin.write(( char *)&buf, sizeofDymOutputType);
	//---------------------------------------
	//xlon	//Patrick 21Oct04
	//---------------------------------------
	for (int i=0;i<nlat;i++)
	{ 
		for (int j=0;j<nlong;j++)
		{
			buf = xlon[i][j];
			ecritbin.write(( char *)&buf, sizeofDymOutputType);
		}
	}
	//---------------------------------------
	//ylat //Patrick 21Oct04
	//---------------------------------------
	for (int i=0;i<nlat;i++)
	{
		for (int j=0;j<nlong;j++)
		{
			buf = ylat[i][j];
			ecritbin.write(( char *)&buf, sizeofDymOutputType);	
		}
	}
	//---------------------------------------
	//zlevel
	//---------------------------------------
	for (int k=0;k<nlevel;k++)
	{
		buf = zlevel[k];
		ecritbin.write(( char *)&buf, sizeofDymOutputType);
	}

	//---------------------------------------
	//msksp
	//---------------------------------------
	for (int i=0;i<nlat;i++)
	{
		for (int j=0;j<nlong;j++)
		{
			bufin = msksp[i][j];
			ecritbin.write(( char *)&bufin,nbytes);
		}
	}
	//---------------------------------------

	ecritbin.close();
}

////////////// ecriture matrice
void CReadWrite::wbin_mat2d(string file_out, const DMATRIX& mat2d, int nlat, int nlong, bool FILEMODE)
{
	ofstream ecritbin;
	if (FILEMODE)
		ecritbin.open(file_out.c_str(), ios::binary|ios::app);
	else
		ecritbin.open(file_out.c_str(), ios::binary|ios::out);

        if (!ecritbin)
        {
                cerr << "Error[" << __FILE__ << ':' << __LINE__ << "]: Unable to write file \"" << file_out << "\"\n";
                exit(1);
        }
                                                                                                                                                                                                     
 	const int sizeofDymOutputType = sizeof(DYM_OUTPUT_MY_TYPE);
	DYM_OUTPUT_MY_TYPE buf;
		
	//---------------------------------------
	// Write 2d matrix
	//---------------------------------------
	for (int i=0;i<nlat;i++)
	{
		for (int j=0;j<nlong;j++)
		{
			buf = mat2d[i][j];
			ecritbin.write(( char *)&buf, sizeofDymOutputType);
		}
	}

	ecritbin.close();
}

//////////////////////////////////////////////////////////
//Ecriture de matrice transposee
/////////////////////////////////////////////////////////////
void CReadWrite::wbin_transpomat2d(string file_out, const DMATRIX& mat2d, int nlong, int nlat, bool FILEMODE)
{
//cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
////////////// ecriture matrice
	ofstream ecritbin;
	if (FILEMODE)
		ecritbin.open(file_out.c_str(), ios::binary|ios::app);
	else
		ecritbin.open(file_out.c_str(), ios::binary|ios::out);

        if (!ecritbin)
        {
                cerr << "Error[" << __FILE__ << ':' << __LINE__ << "]: Unable to write file \"" << file_out << "\"\n";
                exit(1);
        }
                                                                                                                                                                                                     
 	const int sizeofDymOutputType = sizeof(DYM_OUTPUT_TYPE);
	DYM_OUTPUT_TYPE buf;
		
	//---------------------------------------
	// Write 2d matrix
	//---------------------------------------
	for (int j=0;j<nlat;j++)
	{
		for (int i=0;i<nlong;i++)
		{
			buf = (float)mat2d[i][j];
			ecritbin.write(( char *)&buf, sizeofDymOutputType);
		}
	}

	ecritbin.close();
}
/////////////////////////////////////////////////////////////////
////////////// Lecture texte ////////////////
/////////////////////////////////////////////////////////////////
void CReadWrite::rtxt_headpar(string file_in, int &nlong, int &nlat, int &nlevel)
{
cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
exit(1);
	ifstream rtxt;
	rtxt.open(file_in.c_str(), ios::in);
	if (rtxt)
	{
		rtxt >> nlong >> nlat >> nlevel ;
		rtxt.close();
	}
	else
	{
		cout<<endl<<"WARNING : Cannot read file "<<file_in<<endl;
	}
}
///////////////////////////////////////////////////////////////
void CReadWrite::rtxt_header(string file_in, int nlong, int nlat, int nlevel,
						double &startdate, double &enddate,
						DVECTOR& xlon, DVECTOR& ylat, DVECTOR& zlevel, IMATRIX& msksp)
{
cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
exit(1);
	int i,j,k;
	ifstream rtxt;
	rtxt.open(file_in.c_str(), ios::in);
	if (rtxt)
	{
		rtxt >> nlong >> nlat >> nlevel >> startdate >> enddate;
		//---------------------------------------
		//xlon	valeurs des longitudes
		//---------------------------------------
		for (j=0;j<nlong;j++)
			rtxt >> xlon[j] ;
		//---------------------------------------
		//ylat	valeurs des latitudes
		//---------------------------------------
		for (i=0;i<nlat;i++)
			rtxt >> ylat[i];
		//---------------------------------------
		//zlevel: contient les valeurs des dates (yr.decim: yy.dd)
		//---------------------------------------
		for (k=0;k<nlevel;k++)
			rtxt >> zlevel[k];
		//---------------------------------------
		//msksp
		//---------------------------------------
		for (i=0;i<nlat;i++)
			for (j=0;j<nlong;j++)
				rtxt >> msksp[i][j];

		rtxt.close();
	}
	else
	{
		cout<<endl<<"WARNING : Cannot read file "<<file_in<<endl;
	}
}
/////////////////////////////////////////////////////////////////
////////////// Ecriture texte ////////////////
/////////////////////////////////////////////////////////////////
void CReadWrite::wtxt_header(string file_out,int nlong, int nlat, int nlevel,
					double &startdate, double &enddate,
					const DVECTOR& xlon, const DVECTOR& ylat, const DVECTOR& zlevel, const IMATRIX& msksp)
{
cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
exit(1);
////////////// ecriture entete
	int i,j,k;
	ofstream wtxt;
	wtxt.open(file_out.c_str(), ios::out);
	
	if (wtxt)
	{
		//---------------------------------------
		// Write nlong, nlat, nlevel, stardate, enddate
		//---------------------------------------
		wtxt << nlong << '\t'<< nlat << '\t' << nlevel << endl;

		wtxt << startdate << '\t' << enddate << endl;
		//---------------------------------------
		//xlon ylat zlevel
		//---------------------------------------
		for (j=0;j<nlong;j++)
			wtxt << xlon[j] << '\t';
		wtxt << endl;
		for (i=0;i<nlat;i++)
			wtxt << ylat[i] << '\t';
		wtxt << endl;
		for (k=0;k<nlevel;k++)
			wtxt << zlevel[k] << '\t';
		wtxt << endl;
		//---------------------------------------
		//msksp
		//---------------------------------------
		for (i=0;i<nlat;i++)
		{
			for (j=0;j<nlong;j++)
				wtxt << msksp[i][j] << '\t';
			wtxt << endl;
		}
		//---------------------------------------
		wtxt.close();
	}
	else
	{
		cout<<endl<<"WARNING : Cannot write file "<<file_out<<endl;
	}
}
///////////////////////////////////////////////////////////////
void CReadWrite::wtxt_mat2d(string file_out, const DMATRIX& mat2d,
							int	nlat, int nlong, bool FILEMODE)
{
cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
exit(1);
	//	ecriture matrice
	ofstream wtxt ;
	
	if (FILEMODE)
		wtxt.open(file_out.c_str(), ios::app);
	else
		wtxt.open(file_out.c_str(), ios::out);

	if (wtxt)
	{
		for (int i=0;i<nlat;i++)
		{
			for (int j=0;j<nlong;j++)
				wtxt << mat2d[i][j]<<'\t';
			wtxt<< endl;
		}
		wtxt.close();
	}
	else
	{
		cout<<endl<<"WARNING : Cannot write file "<<file_out<<endl;
	}
}
///////////////////////////////////////////////////////////////
// Lecture des formats colonnes lon, lat, var1, var2, ....var n
// il n'y a pas de nom de variable
// ndate donne le numero de variable a lire et a mettre dans
// la matrice mat2d
///////////////////////////////////////////////////////////////
void CReadWrite::rtxt_col_lonlat(string file_in, DMATRIX& mat2d,int nlong, int nlat, DVECTOR& xlon, DVECTOR& ylat, int nbvar, int var)
{
cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
exit(1);
	// LECTURE DES DONNEES COLONNES
	double ilon, jlat;
	int i, j;
	double tempo;
	ifstream rtxt;
	// les matrices sont de type i long x j lat
	//initialisation de la matrice
	for(i=0;i<nlong;i++)
		for (j=0;j<nlat; j++)
			mat2d[i][j]=0.f;
	rtxt.open(file_in.c_str(), ios::in);
	
	if (rtxt)
	{
		while (! rtxt.eof())
		{
			rtxt>> ilon ; // lire longitude dans premiere colonne
			if (ilon<0) ilon=ilon+360;	// cas ou longitude negative a l'EST de 180E
			rtxt >> jlat ; // lire latitude dans deuxieme colonne
			ilon += 0.5;		 // les valeurs long lat des donnees peche correspondent au coin sud-ouest dans la grille
			jlat += 0.5;		 // on corrige pour ramener au centre du degre carre

			int ii=0, jj=0;

			for (i=0; i < nlong-2; i++)	// on compare avec les valeurs de la grille utilisee dans le modele
				if (ilon == xlon[i])	// pour identifier les indices i et j de la matrice
					ii = i;

			for (j=0; j < nlat-2; j++)
				if (jlat == ylat[j])
					jj = j;

			for (int n=1; n<=var  ;n++)
				rtxt >> tempo;	// atteindre la variable en cours

			if ( (ii != 0) && (jj != 0 ) )  // si la longitude et latitude sont dans la grille
			{
				mat2d[ii+1][jj+1]=tempo;			// mettre dans tableau
			}

			for (int n=var+1; n<=nbvar ;n++)	// atteindre la nouvelle longitude - latitude
				rtxt >> tempo;
		}
		rtxt.close();				// ferme le fichier entree
	}
	else
	{
		cout<<endl<<"WARNING : Cannot read file "<<file_in<<endl;
	}
}
void CReadWrite::rtxt_mat2d(string file_in, DMATRIX& mat2d, int &nlong, int &nlat)
{
cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
exit(1);
	ifstream rtxt(file_in.c_str(), ios::in);
	if (rtxt)
	{
		for (int i=0;i<nlat;i++)
			for (int j=0;j<nlong;j++)
			{
				mat2d[i][j]=0.f;
				rtxt >> mat2d[i][j];
			}
			rtxt.close();
	}
	else
	{
		cout<<endl<<"WARNING : Cannot read file "<<file_in<<endl;
	}
}
///////////////////////////////////////////////////////////////
// Converting fishery data files format text to binary
// One file by gear (Longline, purse seine, troll, pole and line, drifnet
// and category deep, shallow mixed, FAD, free school etc...
// format of files generated for sepodym use:
// yyyy   mm    lond   latd   f_effort   skj_mt  yft_mt  bet_mt  alb_mt
// (latitudes south are negative, and long east of 180 can be either negative or >180 )
///////////////////////////////////////////////////////////////
void CReadWrite::wbin_fishery(string file_in, string file_out, int nbvar)
{
cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
exit(1);
	int nbytes= 4;
	double var;
	
	DYM_OUTPUT_TYPE buffl;
	
    	ifstream lit_text;
	ofstream ecritbin;
    
	lit_text.open(file_in.c_str(), ios::in);  // ouvre le fichier entree
	ecritbin.open(file_out.c_str(), ios::binary|ios::out); //ouvre le fichier sortie
	
	if ((lit_text) && (ecritbin))
	{
		while (! lit_text.eof())
		{
			lit_text >> var ;
			buffl = var;
			// if (! lit_text.eof())
			ecritbin.write(( char *)&buffl,nbytes);
		}
		ecritbin.close();
		lit_text.close();		// ferme le fichier entree
	}
	else
	{
		if (!lit_text) cout<<endl<<"WARNING : Cannot read file "<<file_in<<endl;
		if (!ecritbin) cout<<endl<<"WARNING : Cannot write file "<<file_out<<endl;
	}
}

//Inna 05Jul05: routine for reading fishery files header, containing 
//information about uppermost coordinates, dates of records, nb_species
//and positions in file + length of data records at certain date.
void CReadWrite::rbin_fishery_header(CParam& param)
{
	int nbytes = 4;
	float buffl;
	int bufint;
	ifstream litbin;
	float enddate;
	float minlon, maxlon, minlat, maxlat;
	const int nb_fishery = param.get_nbfishery();
	const float start_fishing = param.save_first_yr;
	//const float end_fishing = param.save_last_yr;
	const float end_fishing = param.save_last_yr+param.nb_yr_forecast;
	const int nbt = (int)((end_fishing-start_fishing)*12+1);

	nbsp_file.allocate(0,nb_fishery-1);
	rec_count.allocate(0,nb_fishery-1);
	position_file.allocate(0,nb_fishery-1,0,nbt-1);
	nrecords.allocate(0,nb_fishery-1,0,nbt-1);
	startdate_fishery.allocate(0,nb_fishery-1);
	enddate_fishery.allocate(0,nb_fishery-1);

	nbsp_file.initialize();
	rec_count.initialize();
	position_file.initialize();
	nrecords.initialize();
	startdate_fishery.initialize();
	enddate_fishery.initialize();

	position.allocate(0,nb_fishery-1); 
	numrec.allocate(0,nb_fishery-1);
	all_rec = 0;
//float minLon=360;
//float minLat=180;
//float maxLon=0;
//float maxLat=-180;
	for (int f=0; f<nb_fishery; f++){
		string file_fishery = param.str_dir_fisheries + param.list_file_fishery[f];
		litbin.open(file_fishery.c_str(), ios::binary|ios::in);
		if (litbin){
			litbin.read(( char *)&buffl,nbytes);
			startdate_fishery[f] = buffl;
			litbin.read(( char *)&buffl,nbytes);
			enddate = buffl;
			enddate_fishery[f] = enddate;
			litbin.read(( char *)&bufint,nbytes);
			nbsp_file[f] = bufint;
			//uppermost coordinates are not used
			//at the moment
			litbin.read(( char *)&buffl,nbytes);
			minlon = buffl;
			litbin.read(( char *)&buffl,nbytes);
			maxlon = buffl;
			litbin.read(( char *)&buffl,nbytes);
			minlat = buffl;
			litbin.read(( char *)&buffl,nbytes);
			maxlat = buffl;
//if (minLon>minlon) minLon=minlon;
//if (minLat>minlat) minLat=minlat;
//if (maxLon<maxlon) maxLon=maxlon;
//if (maxLat<maxlat) maxLat=maxlat;
			//---------------------------------
			int nbtoskip = 0;
			int nb_records = nbt;
			if (start_fishing>startdate_fishery[f])
				nbtoskip = (int)((start_fishing-startdate_fishery[f])*12+1);
			if (enddate<end_fishing)
				nb_records -= (int)((end_fishing-enddate_fishery[f])*12+1);
			if (startdate_fishery[f]>start_fishing)
				nb_records -= (int)((startdate_fishery[f]-start_fishing)*12);
			int pos = nbtoskip*nbytes*2;
			litbin.seekg(pos, ios::cur); 

			for(int i=0; i<nb_records; i++){
				litbin.read(( char *)&bufint,nbytes);
				position_file[f][i] = bufint;	
				litbin.read(( char *)&bufint,nbytes);
				nrecords[f][i] = bufint;
				all_rec += nrecords[f][i];	
			}
//cout << f << " " << enddate_fishery[f] << endl;
//cout << start_fishing << " " << startdate_fishery[f] << " " << nbtoskip << " " << nb_records << " " << all_rec << endl;
//cout << param.list_file_fishery[f] << " " << nb_records << " " << position_file[f][0] << " " << enddate_fishery[f]<< endl;
			int nby = (int)end_fishing-(int)start_fishing+1;
			position(f).allocate(0,nby,1,12);
			numrec(f).allocate(0,nby, 1,12);
			position(f).initialize();
			numrec(f).initialize();
			litbin.close();
		} else 
			cout << endl << "WARNING : Cannot read file " << param.list_file_fishery[f].c_str() << endl;
	}
//cout << minLon << " " << minLat << " " << maxLon << " " << maxLat << endl;
}

void CReadWrite::read_fishery_data(CParam& param, const PMap& map)
{
	const int nb_fishery = param.get_nbfishery();
	const float start_fishing = param.save_first_yr;
	//const float end_fishing = param.save_last_yr;
	const float end_fishing = param.save_last_yr+param.nb_yr_forecast;
	
	frec = new fishery_record[all_rec];

	fisheries_data_read = true;
	int nbytes = 4;
	float buffl;
	int bufint;
	ifstream litbin;
	
	int npr = 0;

	for (int f=0; f<nb_fishery; f++){
		if (param.mask_fishery_sp[0][f]){
			int month = (int)((start_fishing - (int)start_fishing)*12+1);
			int year  = 0;
			if (startdate_fishery[f]>start_fishing)
				year += (int)(startdate_fishery[f]-start_fishing);
//cout << year << endl;
			string file_fishery = param.str_dir_fisheries + param.list_file_fishery[f];
			litbin.open(file_fishery.c_str(), ios::binary|ios::in);
			if (litbin){
				litbin.read(( char *)&buffl,nbytes);
				float startdate = buffl;
				litbin.read(( char *)&buffl,nbytes);
				float enddate = buffl;
				//int nb_series = (int)((min(end_fishing,enddate)-max(start_fishing,startdate))*12+1);
				int nb_series = (int)((min(end_fishing,enddate)-max(start_fishing,startdate))*12);
//cout << "reading..." << file_fishery << " " <<  position_file[f][0] << " " << nb_series << " " <<  month << endl;
				litbin.seekg(position_file[f][0], ios::beg);
				for (int k=0; k<nb_series; k++){
					if (position_file[f][0]==0)
						litbin.seekg(position_file[f][k], ios::beg);		
					position(f,year,month) = npr;
					int nrec = nrecords[f][k];
					int nrec_oceanmask = 0;	
					for (int nn=0; nn<nrec; nn++){
						litbin.read(( char *)&bufint,nbytes);
						int i = bufint;
//cout << nn << " " << i << " ";
//if (nn==0) cout << "reading..." << file_fishery << " " << nrec << " " << i << endl;
						litbin.read(( char *)&bufint,nbytes);
						int j = bufint;
						litbin.read(( char *)&buffl,nbytes);
						double eff = buffl;
						int nb_species = nbsp_file[f];
						dvector harv(0,nb_species-1);
						harv.initialize();
						for (int sp=0; sp<nb_species; sp++){
							litbin.read(( char *)&buffl,nbytes);
							harv[sp] = buffl;
						}
						if (map.carte(i,j) && eff){
						//if (map.carte(i,j) && eff && j>=18){//only tropics on 2deg grid
							frec[nrec_oceanmask+npr].set_record(i,j,eff,harv);
							nrec_oceanmask++;

						}
	
					}
					numrec(f,year,month) = nrec_oceanmask;
					month++; if (month>12) {month=1; year++;}
					npr += nrec_oceanmask;
//if (k==nb_series-1) cout << month << " " << year << " " << npr << endl;
				}
	
				litbin.close();
			} else {
				cout << endl << "WARNING : Cannot read file " << file_fishery.c_str() << endl;
			}
		}
	}
}

void CReadWrite::write_fishery_data(CParam& param, const PMap& map, CMatrices& mat, const int sp, const int year, const int m, bool FILEMODE)
{
	const int nb_fishery = param.get_nbfishery();
	const int nb_species = param.get_nbspecies();
	const float start_fishing = param.save_first_yr;
	const float end_fishing = param.save_last_yr;
	const int y = year-(int)start_fishing;
	
	int nbytes = 4;
	float buffl;
	int bufint;
	ofstream ecritbin;
	
	int k = 0;
	for (int f=0; f<nb_fishery; f++){
		if (param.mask_fishery_sp[sp][f]){
			string file_out = param.str_dir_fisheries + "pred_"+ param.list_file_fishery[f];
			if (!FILEMODE){
				ecritbin.open(file_out.c_str(), ios::binary|ios::out);
	        		if (!ecritbin) {
			                cerr << "Error[" << __FILE__ << ':' << __LINE__ << "]: Unable to write file \"" << file_out << "\"\n";
					exit(1);
			        }
				//Writing header
				buffl = max(start_fishing,(float)startdate_fishery[f]);
				ecritbin.write(( char *)&buffl,nbytes);
				buffl = min(end_fishing,(float)enddate_fishery[f]);
				ecritbin.write(( char *)&buffl,nbytes);
				bufint = nb_species;
				ecritbin.write(( char *)&bufint,nbytes);
				buffl = param.longitudeMin;
				ecritbin.write(( char *)&buffl,nbytes);
				buffl = param.longitudeMax;
				ecritbin.write(( char *)&buffl,nbytes);
				buffl = param.latitudeMin;
				ecritbin.write(( char *)&buffl,nbytes);
				buffl = param.latitudeMax;
				ecritbin.write(( char *)&buffl,nbytes);
				//int nbt_file = (int)(enddate_fishery[f]-startdate_fishery[f])*12;
				int nbt = (int)((min(end_fishing,(float)enddate_fishery[f])-max(start_fishing,(float)startdate_fishery[f]))*12);
				int rec_shift = nbytes * (7 + nbt * 2);
				int M = (int)((start_fishing - (int)start_fishing)*12+1);
				int Y = 0;
				int pos_file = rec_shift;
//cout << "writing..." << param.list_file_fishery[f] << " " << nbt << " " << rec_shift << " " << start_fishing << " " << end_fishing << endl;
				for (int t=0; t<nbt; t++){
					bufint = pos_file;
					ecritbin.write(( char *)&bufint,nbytes);
					bufint = numrec(f,Y,M);
					ecritbin.write(( char *)&bufint,nbytes);
					pos_file += numrec(f,Y,M)*(3+nb_species)*nbytes;
					M++; if (M>12) {M=1; Y++;}
				}
				ecritbin.close();
			}	
			else {
				ecritbin.open(file_out.c_str(), ios::binary|ios::app);
//cout << "writing data" << " " << ecritbin.tellp() << endl; 
				//writing predicted catch data
//cout << "writing..." << param.list_file_fishery[f] << " " << numrec(f,y,m) << " " << frec[position(f,y,m)].get_i() << endl;
				int nstart = position(f,y,m);
				int nend = nstart+numrec(f,y,m); 
				for (int p=nstart; p<nend; p++){
					int i = frec[p].get_i();
					int j = frec[p].get_j();
					bufint = i; 
					ecritbin.write(( char *)&bufint, nbytes);
					bufint = j; 
					ecritbin.write(( char *)&bufint, nbytes);
					buffl = mat.effort[f][i][j];
					ecritbin.write(( char *)&buffl, nbytes);
					buffl = mat.catch_est[sp][k][i][j];
					ecritbin.write(( char *)&buffl, nbytes);
				} 
				ecritbin.close();
			}
			k++;
			
		}
	}
}

void CReadWrite::delete_fisheries_rec() 
{ 
	if (fisheries_data_read)
		delete [] frec; 
}

void CReadWrite::get_fishery_data(CParam& param, D3_ARRAY& effort, D4_ARRAY& catch_obs, int y, const int m)
{	
	y -= (int)param.save_first_yr;
	effort.initialize();
	catch_obs.initialize();
	int k = 0;
	const int nb_fishery = param.get_nbfishery();

	for (int f=0; f<nb_fishery; f++){
		if (param.mask_fishery_sp[0][f]){
			int nstart = position(f,y,m);
			int nend = nstart+numrec(f,y,m); 
			for (int p=nstart; p<nend; p++){
				int i = frec[p].get_i();
				int j = frec[p].get_j();
				effort(f,i,j) = frec[p].get_effort();
//if (y>9)
//	effort(f,i,j) *= 2.0;
				int nb_species = nbsp_file[f];	
				dvector harv(0,nb_species-1);
				harv = frec[p].get_catch(); 
				for (int sp=0; sp<nb_species; sp++){
					catch_obs(sp,k,i,j) = harv[sp];
				}				
			} k++;
//cout << sum(catch_obs(0,k-1)) << endl;
		}
	}
}

void CReadWrite::get_average_effort(CParam& param, D3_ARRAY& effort, D4_ARRAY& catch_obs, const int nby, const int m)
{	
	int y1 = (int)param.save_last_yr-(int)param.save_first_yr-nby;
	int y2   = y1+nby;

	effort.initialize();
	catch_obs.initialize();
	int k = 0;
	const int nb_fishery = param.get_nbfishery();

	for (int f=0; f<nb_fishery; f++){
		if (param.mask_fishery_sp[0][f]){
			for (int y=y1; y<y2; y++){
				int nstart = position(f,y,m);
				int nend = nstart+numrec(f,y,m); 
				for (int p=nstart; p<nend; p++){
					int i = frec[p].get_i();
					int j = frec[p].get_j();
					effort(f,i,j) += frec[p].get_effort();
					
				} k++;
			}
			effort(f) /= nby;	 
			//effort(f) = 2.0*effort(f);
		}
	}
}

void CReadWrite::get_effort(CParam& param, dmatrix& effort, const int f, int y, const int m)
{	
	effort.initialize();
	y -= (int)param.save_first_yr;
	int nstart = position(f,y,m);
	int nend = nstart+numrec(f,y,m); 
	for (int p=nstart; p<nend; p++){
		int i = frec[p].get_i();
		int j = frec[p].get_j();
		effort(i,j) = frec[p].get_effort();
	}
}

void CReadWrite::read_lf_WCPO(CParam& param, string filename, const float startdate, const float enddate, const int sp)
{ //data used by Multifan-CL, file changed only in header

	const int nb_fishery = param.get_nbfishery();
	const int nb_ages    = param.sp_nb_age_class_ad[sp];

	ifstream littxt(filename.c_str());
	if (littxt){
		int nb_regions, nb_fleets, nb_records, nb_intervals, l1, dl;
		float w1, wl;
		string* fcodes;

		littxt >> nb_regions >> nb_fleets >> nb_records;
		ivector region(0,nb_fleets-1);
		region.initialize();
		fcodes = Utilities::create1d(fcodes, nb_fleets);

		for (int f=0; f<nb_fleets; f++)
			littxt >> fcodes[f];
		for (int f=0; f<nb_fleets; f++)
			littxt >> region[f];

		//read the line 
		//flexible to different file contents, namely to the presence of weight data
		string s;
		getline(littxt,s,'\n');
		getline(littxt,s,'\n');
		std::istringstream iss(s);
		iss >> nb_intervals >> l1 >> dl;

		dvector lf(0,nb_intervals-1);
		lf.initialize();
		ivector Length(0,nb_intervals-1);
		for (int l=0; l< nb_intervals; l++)
			Length(l) = (int)(l1+dl*(l+0.5)); 
			//Length(l) = (int)(l1+dl*l); 

		//skip the line
		getline(littxt,s,'\n');
		getline(littxt,s,'\n');
		int year, month, week, f_num;
		float E, C;

		for (int n=0; n<nb_records; n++){
			littxt >> year >> month >> week >> f_num >> C >> E;
			float frequency;
			lf.initialize();
			//read the whole line (some files contain weight frequencies data)
			getline(littxt,s,'\n');
			std::istringstream iss(s);
			for (int l=0; l<nb_intervals; l++){
				iss >> frequency;
				if (frequency == -1) break;
				lf[l] = frequency;	
			}
			bool fishery_to_throw = true;
			int f;
			for (f=0;f<nb_fishery;f++){
				if (param.list_fishery_name[f] == fcodes[f_num-1]){
					fishery_to_throw = false; 
					break;
				}
				
			}
			if (fishery_to_throw) continue;
			float date = year+month/12.0;
			if ((date >= startdate) && (date <=enddate)){
				int qtr = month/3;
				double L_pr = param.juv_length(sp,param.sp_nb_age_class_jv[sp]-1);
				int y=year-(int)startdate;
				double right;
				for (int a=0; a< nb_ages-1; a++){

					double left  = 0.5*(L_pr+param.length(sp,a));
					right = 0.5*(param.length(sp,a)+param.length(sp,a+1));
					L_pr = param.length(sp,a);

					for (int l=0; l<nb_intervals; l++){
						if ((Length[l]>left) && (Length[l]<=right))
							frq(f,region(f_num-1)-1,y,qtr,a) += lf[l];
					}
				}
				for (int l=0; l<nb_intervals; l++)
					if ((Length[l]>right)&& (Length[l]<param.length[sp][nb_ages-1]+10))
						frq(f,region(f_num-1)-1,y,qtr,nb_ages-1) += lf[l];
			}
		}
		littxt.close();
	} else {
		cout << endl << "WARNING : Cannot read file " << filename.c_str() << endl;
	}
//exit(1);
}

void CReadWrite::read_lf_EPO(CParam& param, string filename, const float startdate, const float enddate, const int sp)
{//data provided by IATTC, file changed only in header

	const int nb_fishery = param.get_nbfishery();
	const int nb_ages    = param.sp_nb_age_class_ad[sp];

	ifstream littxt(filename.c_str());
	//ifstream littxt("./skj_LF_IATTC.txt");
	if (littxt){
		int region, nb_fleets, nb_records, nb_intervals, l1, dl;
		string* fcodes;

		littxt >> region >> nb_fleets >> nb_records;
		fcodes = Utilities::create1d(fcodes, nb_fleets);

		for (int f=0; f<nb_fleets; f++)
			littxt >> fcodes[f];

		littxt >> nb_intervals >> l1 >> dl;
		dvector lf(0,nb_intervals-1);
		lf.initialize();
		dvector Length(0,nb_intervals-1);
		for (int l=0; l< nb_intervals; l++)
			Length(l) = l1+(float)dl*(l+0.5); 
		string s;
		getline(littxt,s,'\n');
		getline(littxt,s,'\n');

		int year, qtr, f_num;
		for (int n=0; n<nb_records; n++){
			littxt >> year >> qtr >> f_num >> s;

			int frequency;
			lf.initialize();

                        for (int l=0; l<nb_intervals; l++){
                        	littxt >> frequency;
                                lf[l] = frequency;
                        }

			bool fishery_to_throw = true;
			int f;
			for (f=0;f<nb_fishery;f++){
				if (param.list_fishery_name[f] == fcodes[f_num-1]){
					fishery_to_throw = false; 
					break;
				}
				
			}
			if (fishery_to_throw) continue;

			float date = year+(qtr*3.0-1)/12;
			if ((date >= startdate) && (date <=enddate)){
				double L_pr = param.juv_length(sp,param.sp_nb_age_class_jv[sp]-1);
				int y=year-(int)startdate;
				double right;
				for (int a=0; a< nb_ages-1; a++){
					double left  = 0.5*(L_pr+param.length(sp,a));
					right = 0.5*(param.length(sp,a)+param.length(sp,a+1));
					L_pr = param.length(sp,a);

					for (int l=0; l<nb_intervals; l++){
						if ((Length[l]>left) && (Length[l]<=right))
							frq(f,region-1,y,qtr-1,a) += lf[l];
					}
				}
				for (int l=0; l<nb_intervals; l++)
					if ((Length[l]>right)&& (Length[l]<param.length[sp][nb_ages-1]+10))
						frq(f,region-1,y,qtr-1,nb_ages-1) += lf[l];
//if (f==5 && qtr==1 && region==7) cout << frq(f,region-1,y,qtr-1) << endl;
//cout << f << " "<< region << " " << y << " "<< qtr << " " << frq(f,region-1,y,qtr) << endl; exit(1);
			}
		}
		littxt.close();
	} else {
		cout << endl << "WARNING : Cannot read file " << filename.c_str() << endl;
	}



}

void CReadWrite::read_frq_data(CParam& param, const float startdate, const float enddate, const int sp)
{
	const int nby = (int)(enddate) -(int)(startdate)+1;
	const int nbq = 4; 
	int nb_fishery = param.get_nbfishery();
	int nb_region  = param.nb_region;
	int nb_ages    = param.sp_nb_age_class_ad[sp];

	frq.allocate(0,nb_fishery-1,0,nb_region-1,0,nby-1,0,nbq-1,0,nb_ages-1);
	frq.initialize();

	//if (param.flag_twin) {
	//temporally:
	if (param.nb_frq_files==1 && nb_region<8) {
		read_pred_frq_data(param,param.file_frq_data[0],startdate,enddate,sp);
		return;
	}
	read_lf_WCPO(param,param.file_frq_data[0],startdate,enddate,sp);
	if (param.nb_frq_files==2)
		read_lf_EPO(param,param.file_frq_data[1],startdate,enddate,sp);
	
	//write the file with observations
	d4_array sum_frq(0,nb_fishery-1,0,nb_region-1,0,nbq,0,nb_ages-1);
	d3_array sum_frq_fishery(0,nb_fishery-1,0,nbq,0,nb_ages-1);
	d3_array sum_frq_region(0,nb_region-1,0,nbq,0,nb_ages-1);
	sum_frq.initialize();
	sum_frq_fishery.initialize();
	sum_frq_region.initialize();

	for (int f=0; f<nb_fishery; f++){
		for (int r=0; r<nb_region; r++){
			for (int q=0; q<nbq; q++){
				for (int y=0; y<nby; y++){
					if (norm(frq(f,r,y,q))){
						//cout << param.list_fishery_name[f] << "_region_" << r+1 << " year" << y << " qtr" << q+1 << " "; 
						for (int a=0; a< nb_ages; a++){
							//cout << frq(f,r,y,q,a) << " ";				
							sum_frq(f,r,q,a) += frq(f,r,y,q,a);
							sum_frq_fishery(f,q,a) += frq(f,r,y,q,a);
							sum_frq_region(r,q,a) += frq(f,r,y,q,a);
						//cout << endl;
						}
					}
				}
			}
			for (int q=0; q<nbq; q++){
				for (int a=0; a< nb_ages; a++){
					sum_frq(f,r,nbq,a) += sum_frq(f,r,q,a);  
					sum_frq_fishery(f,nbq,a) += sum_frq_fishery(f,q,a);
					sum_frq_region(r,nbq,a) += sum_frq_region(r,q,a);
				}
			}
		}
	}
	//write the file with read observations aggregated over Seapodym's age classes during simulation period
	string filename	= param.strdir_output + param.sp_name[0] + "_LF_obs.txt";
	ofstream ecritLF(filename.c_str());

	if (ecritLF) {
		ecritLF << "length" <<'\t' ;
		for (int reg=0; reg< param.nb_region_sp_B[sp]; reg++)
			for (int f=0; f< nb_fishery; f++)
				if (param.mask_fishery_sp[sp][f])
					ecritLF<<param.list_fishery_name[f]<<"_"<<param.sp_name[sp]<<"_region_"<<param.area_sp_B[sp][reg] <<'\t';
			
		for (int f=0; f< nb_fishery; f++)
			if (param.mask_fishery_sp[sp][f])
				ecritLF<<"sum_" << param.list_fishery_name[f]<<"_"<<param.sp_name[sp]<<'\t';
					

		for (int reg=0; reg< param.nb_region_sp_B[sp]; reg++)	
			ecritLF<<"sum_" << param.sp_name[sp]<<"_region_"<<param.area_sp_B[sp][reg] <<'\t';		

		ecritLF << endl;
		
		for (int q=0; q<=nbq; q++){
			ecritLF <<"Quarter "<< q+1 << endl;
			for (int a=0; a< nb_ages; a++){
				ecritLF << param.length(sp,a) << '\t';
				for (int r=0; r< param.nb_region_sp_B[sp]; r++)
					for (int f=0; f< nb_fishery; f++)
						if (param.mask_fishery_sp[sp][f]){
							int reg = param.area_sp_B[sp][r]-1;
							ecritLF << sum_frq(f,reg,q,a) << '\t'; 
						}
				for (int f=0; f< nb_fishery; f++)
					if (param.mask_fishery_sp[sp][f])				
						ecritLF << sum_frq_fishery(f,q,a) << '\t';

				for (int r=0; r< param.nb_region_sp_B[sp]; r++){
					int reg = param.area_sp_B[sp][r]-1;
					ecritLF << sum_frq_region(reg,q,a) << '\t';
				}

				ecritLF << endl;
					
			}
		}
			
		ecritLF  << endl;
		ecritLF.close();
	}
	else
	{
		cout<<endl<<"WARNING : Cannot write file "<< filename.c_str()<<endl;
	}
}

void CReadWrite::write_frq_data(CParam& param, int sp, int year, int qtr, d3_array frq, bool FILEMODE)
{
	int nb_ages = param.sp_nb_age_class_ad[sp];
	int nb_fishery = param.nb_fishery_by_sp(sp);
	int nb_region  = param.nb_region;


	string filename	= param.str_dir_fisheries + "skj_LF_pred.txt";
	ofstream ecritLF;
	if (!FILEMODE){
		ecritLF.open(filename.c_str(), ios::out);
		if (ecritLF) {
			ecritLF << "year" <<'\t' << "quarter" <<'\t'<< "fisheries"<<'\t'<<"region" <<'\t';
			for (int a=0; a<nb_ages; a++)
				ecritLF << a <<'\t';
			ecritLF << endl;
		}
		else cout<<endl<<"WARNING : Cannot write file "<< filename.c_str()<<endl;
	}
	else {
		ecritLF.open(filename.c_str(), ios::app);
		if (ecritLF) {
			for (int f=0; f<nb_fishery; f++){
				for (int r=0; r<nb_region; r++){
					int reg = param.area_sp_B[sp][r]-1;
					ecritLF << year << '\t' << qtr <<'\t'<< f <<'\t'<< reg <<'\t';
					for (int a=0; a<nb_ages; a++)
						ecritLF << frq(a,f,reg) <<'\t';
					ecritLF << endl;
				}
			}
		}
		else cout<<endl<<"WARNING : Cannot write file "<< filename.c_str()<<endl;
	}
}

void CReadWrite::read_pred_frq_data(CParam& param, string filename, const float startdate, const float enddate, const int sp)
{
	int nb_ages = param.sp_nb_age_class_ad[sp];
	int nb_fishery = param.nb_fishery_by_sp(sp);
	int nb_region  = param.nb_region;

	ifstream littxt(filename.c_str());
	if (littxt){
		string s;
		getline(littxt,s,'\n');
		
		int year, qtr, f, reg;
		int nb_records = (int)((enddate-startdate)*4)*nb_fishery*nb_region;
		for (int n=0; n<nb_records; n++){
			littxt >> year >> qtr >> f >> reg;
			int y = year-(int)startdate;
			for (int a=0; a<nb_ages; a++)
				littxt >> frq(f,reg,y,qtr-1,a);
		}	
	}
	else cout<<endl<<"WARNING : Cannot read file "<< filename.c_str()<<endl;
}


void CReadWrite::get_LF_qtr_data(CParam& param, d4_array LF_qtr_obs, int y, const int q)
{
	LF_qtr_obs.initialize();
	int nb_species = param.get_nbspecies();
	int nb_fishery = param.get_nbfishery();
	y -= (int)param.save_first_yr;

	for (int sp=0; sp<nb_species; sp++){
		int nb_ages = param.sp_nb_age_class_ad[sp];
		int nb_region = param.nb_region_sp_B[sp];
		int k=0;
		for (int f=0; f<nb_fishery; f++)
			if (param.mask_fishery_sp[sp][f]){
				for (int age=0; age< nb_ages; age++)
					for (int r=0; r<nb_region; r++){
						int reg = param.area_sp_B[sp][r]-1;
						LF_qtr_obs(sp,age,k,r) = frq(f,reg,y,q,age); 
//cout << LF_qtr_obs(sp,age,k,r) << " ";
					}
				k++;
			}
	}	
//cout << endl;
}

////////////////////////////////////
void CReadWrite::InitSepodymFileDym(CParam& param, CMatrices& mat, int nb_mo, DVECTOR& zlevel, const IMATRIX& msksp)
{
	double minval = (double) 3.4e38;
	double maxval = (double) -3.4e38;
	const int nb_species = param.get_nbspecies();
	const int nb_fishery = param.get_nbfishery();

	// Recording 2 variables by forage component:
	// 0- Fprod predicted spatial distribution of forage production
	// 1- Fbiom predicted spatial distribution of forage biomass
	//variable name
//	int nb_var_F= 1;
//	vector<string> var_F_name;
//	//var_F_name.push_back("Fprod");
//	var_F_name.push_back("Fbiom");
//
//	for (int n=0; n<nb_forage;n++){
//		for (int var=0; var<nb_var_F;var++){
//			//Generate the name of the files
//			dymFileFPred.push_back(param.strdir_output+var_F_name[var]+"_"+param.frg_name[n]+".dym");
//			// Call the function to write the header of each file
//			wbin_header(dymFileFPred[var+(nb_var_F*n)], param.idformat, param.idfunc, minval, maxval, 
//				param.nlong, param.nlat, nb_mo, zlevel[0], zlevel[nb_mo -1],
//				mat.xlon, mat.ylat, zlevel, msksp);
//		}
//	}
	vector<string> file_name;
	int Nfiles = 5; //6
	//file_name.push_back("Residuals.dym");
	file_name.push_back("Young_Habitat.dym");
	file_name.push_back("Adult_Habitat.dym");
	file_name.push_back("Advection_x.dym");
	file_name.push_back("Advection_y.dym");
	file_name.push_back("Diffusion.dym");
	
	for (int var=0; var<Nfiles;var++){
		dymFileFPred.push_back(param.strdir_output+file_name[var]);
		wbin_header(dymFileFPred[var], param.idformat, param.idfunc, minval, maxval, 
				param.nlong, param.nlat, nb_mo, zlevel[0], zlevel[nb_mo -1],
				mat.xlon, mat.ylat, zlevel, msksp);
	}

	// Recording 6 variables by species:
	int nb_var= 6;
	vector<string> var_name;
	var_name.push_back("larve");
	var_name.push_back("juvnl");
	var_name.push_back("young");
	var_name.push_back("recru");
	var_name.push_back("adult");
	var_name.push_back("totbm");
	if (nb_species)
	{
		for (int sp=0; sp<nb_species;sp++)
		{	
			for (int var=0; var<nb_var;var++)
			{
				//Generate the name of the file
				dymFileSpPred.push_back(param.strdir_output+param.sp_name[sp]+"_"+var_name[var]+".dym");
				// Call the function to write the header of each file
				wbin_header(dymFileSpPred[var+(nb_var*sp)], param.idformat, param.idfunc, minval, maxval, 
					param.nlong, param.nlat, nb_mo, zlevel[0], zlevel[nb_mo -1],
					mat.xlon, mat.ylat, zlevel, msksp);
			}
		}
	}
	// Recording n variable [nb_fishery_type_sp[sp]] for observed catch
	// number of files
	int nb_type_sp=0;
	if (nb_fishery)
	{
		for (int sp =0; sp<nb_species;sp++)
		{
			for (int n=0; n<param.nb_fishery_type_sp[sp];n++)
			{
				nb_type_sp++ ;
			}
		}
	}

	int nfiles = 0;
	if (nb_fishery)	{
		//Generate the name of the files
		for (int sp =0; sp<nb_species;sp++){
			dymFileSpCobs.push_back(param.strdir_output+param.sp_name[sp]+"_Cobs.dym");
			dymFileSpCobs.push_back(param.strdir_output+param.sp_name[sp]+"_Cpred.dym");
			nfiles += 2;
			for (int f=0;f<nb_fishery; f++){
				if (param.mask_fishery_sp[sp][f]){
					dymFileSpCobs.push_back(param.strdir_output + "R_" + param.sp_name[sp]+"_" + param.list_fishery_name[f] +".dym");
					//dymFileSpCobs.push_back(param.strdir_output+param.sp_name[sp]+"_" + param.list_fishery_name[f]+"_obs.dym");
					//dymFileSpCobs.push_back(param.strdir_output+param.sp_name[sp]+"_" + param.list_fishery_name[f]+"_pred.dym");
					//nfiles += 2; 
					nfiles ++;
				} 
			}
		}
	}
	// Call the function to write the header of each file
	if (nb_type_sp>0){
		for (int nf=0; nf<nfiles;nf++){

			wbin_header(dymFileSpCobs[nf], param.idformat, param.idfunc, minval, maxval, 
				param.nlong, param.nlat, nb_mo,	zlevel[0], zlevel[nb_mo -1], 
				mat.xlon, mat.ylat, zlevel, msksp);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void CReadWrite::SaveSepodymFileDym(CParam& param, PMap& map, CMatrices& mat)
{	
	const int nlon = param.nlong;
	const int nlat = param.nlat;
	const int nb_species = param.get_nbspecies();
	const int nb_fishery = param.get_nbfishery();

	// SAVE FORAGE
//	for (int n=0; n< nb_forage; n++){
//
//		SaveDymFile(map, dymFileFPred[n], mat.forage[n], nlon, nlat);
//	}
//
//	SaveDymFile(map, mat, dymFileFPred[0], mat.Hs[0], nlon, nlat);
//	SaveDymFile(map, mat, dymFileFPred[1], mat.Hj[0], nlon, nlat);
//	SaveDymFile(map, mat, dymFileFPred[2], mat.Ha[0], nlon, nlat);
//	SaveDymFile(map, mat, dymFileFPred[3], mat.Ha[1], nlon, nlat);
//	SaveDymFile(map, mat, dymFileFPred[4], mat.Ha[2], nlon, nlat);
//	SaveDymFile(map, mat, dymFileFPred[5], mat.Ha[3], nlon, nlat);
	int ixv = 0;
	//SaveDymFile(map, mat, dymFileFPred[ixv], mat.residuals, nlon, nlat); ixv++;
	SaveDymFile(map, mat, dymFileFPred[ixv], mat.Hj[0], nlon, nlat); ixv++;
	SaveDymFile(map, mat, dymFileFPred[ixv], mat.Ha[0], nlon, nlat); ixv++;
	SaveDymFile(map, mat, dymFileFPred[ixv], mat.advection_x, nlon, nlat); ixv++;
	//SaveDymFile(map, mat, dymFileFPred[ixv], mat.Hs[0], nlon, nlat); ixv++;
	SaveDymFile(map, mat, dymFileFPred[ixv], mat.advection_y, nlon, nlat); ixv++;
	//SaveDymFile(map, mat, dymFileFPred[ixv], mat.Hj[0], nlon, nlat); ixv++;
	SaveDymFile(map, mat, dymFileFPred[ixv], mat.diffusion_y, nlon, nlat); ixv++;
	//SaveDymFile(map, mat, dymFileFPred[ixv], mat.Ha[0], nlon, nlat); ixv++;

	// SAVE SPECIES DISTRIBUTIONS
	int nb_var =6;  
	for (int sp=0;sp<nb_species;sp++)	{
		
		// 0- larvae
		string filename = dymFileSpPred[nb_var*sp];
		SaveDymFile(map, mat, filename, mat.larvae[sp], nlon, nlat);
		//SaveDymFile(map, mat, filename, mat.Hj[sp], nlon, nlat);
		
		// 1- juvenile
		filename = dymFileSpPred[1+(nb_var*sp)];
		SaveDymFile(map, mat, filename, mat.juvenile[sp], nlon, nlat);
		//SaveDymFile(map, mat, filename, mat.Ha[sp], nlon, nlat);

		// 2- young
		filename = dymFileSpPred[2+(nb_var*sp)];
		SaveDymFile(map, mat, filename,  mat.young[sp], nlon, nlat);

		// 3- recruit
		filename = dymFileSpPred[3+(nb_var*sp)];
		//SaveDymFile(map, mat, filename,  mat.recruit[sp], nlon, nlat);
		SaveDymFile(map, mat, filename, mat.Hj[sp], nlon, nlat);

		// 4- adults
		filename = dymFileSpPred[4+(nb_var*sp)];
		SaveDymFile(map, mat, filename,  mat.adult[sp], nlon, nlat);

		// 5- total population
		filename = dymFileSpPred[5+(nb_var*sp)];
		SaveDymFile(map, mat, filename,  mat.total_pop[sp], nlon, nlat);
	

		// PREDICTED AND OBSERVED CATCH
		if (nb_fishery){
			int ixv = 0;
			SaveDymFile(map, mat, dymFileSpCobs[ixv],  mat.total_obs_catch[sp],  nlon, nlat); ixv++;
			SaveDymFile(map, mat, dymFileSpCobs[ixv],  mat.total_pred_catch[sp], nlon, nlat); ixv++;
			int k = 0;
			for (int f=0;f<nb_fishery; f++){
				if (param.mask_fishery_sp[sp][f]){
					SaveDymFile(map, mat, dymFileSpCobs[ixv],  mat.residuals[k],  nlon, nlat);
					//SaveDymFile(map, mat, dymFileSpCobs[ixv],  mat.catch_obs[sp][k],  nlon, nlat);
					//SaveDymFile(map, mat, dymFileSpCobs[ixv],  mat.catch_est(sp,k),  nlon, nlat);

					//SaveDymFile(map, mat, dymFileSpCobs[ixv*2],  mat.catch_obs[sp][k],  nlon, nlat);
					//SaveDymFile(map, mat, dymFileSpCobs[ixv*2+1],  mat.catch_est(sp,k),  nlon, nlat);
					ixv++; k++;
				} 
			}

		}
	}
}

void CReadWrite::SaveDymFile(PMap& map, CMatrices& mat, string file, const dmatrix& data, const int nlon, const int nlat)
{
	bool FileMode = true; 
	double minval = 3.4e38;
	double maxval = -3.4e38;

	mat2d_NoBorder.shallow_copy(mat.mat2d_NoBorder);
	mat2d_NoBorder.initialize();

	for (int i=map.imin; i <= map.imax; i++){
		for (int j=map.jinf[i] ; j<=map.jsup[i] ; j++){
			if (map.carte[i][j]){
				mat2d_NoBorder(i-1,j-1) = data(i,j);
				if (minval>data(i,j)) minval = (DYM_OUTPUT_TYPE)data(i,j);
				if (maxval<data(i,j)) maxval = (DYM_OUTPUT_TYPE)data(i,j);
			}
		}
	}
//if (file=="run4804x30dx2deg/output/simout/Young_Habitat.dym") cout << minval << " " << maxval << endl;
	wbin_transpomat2d(file, mat2d_NoBorder,	nlon, nlat, FileMode);
	rwbin_minmax(file, minval, maxval);
}

//////////////////////////////////////////////////////////
// ECRITURE EN-TETE FICHIERS DE SAUVEGARDE SEPODYM 'TEXT'
//////////////////////////////////////////////////////////
void CReadWrite::InitSepodymFileTxt(CParam& param)
{
//cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
	//////////////////////////////////////
	//WRITE HEADERS OF SUMDYM.TXT
	//ecriture des noms de variable du fichier SumDym.txt
	// fichier sommes mensuelles
	string fileSumDym	= param.strdir_output + "SumDym.txt";
	ofstream ecritSumDym;
	ecritSumDym.open(fileSumDym.c_str(), ios::out);

	const int nb_species = param.get_nbspecies();
	const int nb_fishery = param.get_nbfishery();
	const int nb_forage = param.get_nbforage();

	if (ecritSumDym)
	{
		ecritSumDym <<"date"<< '\t'<<"tstep"<< '\t' << "P" <<'\t';
		for(int n = 0; n< nb_forage;n++)
		{
			ecritSumDym <<"Fprime_"<<param.frg_name[n]<<'\t'
				<<"F_"<<param.frg_name[n]<<'\t'
				<<"F_"<<param.frg_name[n]<<" in pred. area"<<'\t'
				<<"F_"<<param.frg_name[n]<<" required by pred."<<'\t'
				<<"Mean Omega_"<<param.frg_name[n]<<'\t' ;
		}
		for (int sp=0; sp<nb_species;sp++)
		{
			ecritSumDym <<"B Juv. "<<param.sp_name[sp] <<'\t'
				<<"B Rec "<<param.sp_name[sp] <<'\t'
				<<"B Young "<<param.sp_name[sp] <<'\t'
				<<"B Adult "<<param.sp_name[sp] <<'\t'
				<<"B Tot."<<param.sp_name[sp] <<'\t';
		}

		if (nb_fishery){
			for (int f=0;f<nb_fishery; f++){
				for (int sp=0;sp<nb_species;sp++){
					if (param.mask_fishery_sp[sp][f]){

						ecritSumDym <<"effort " << param.list_fishery_name[f] << '\t' ;
						ecritSumDym <<"obs C_"  << param.sp_name[sp]<<"_"<<param.list_fishery_name[f] <<'\t' ;
						ecritSumDym <<"pred C_" << param.sp_name[sp]<<"_"<<param.list_fishery_name[f] <<'\t' ;
					}
				}
			}
		}
		ecritSumDym << endl;
	}
	else
	{
		cout<<endl<<"WARNING : Cannot write file "<<fileSumDym<<endl;
	}

	//////////////////////////////////////
	//WRITE HEADERS OF SUMQAREA.TXT
	//ecriture des noms de variable du fichier SumQArea.txt
	// fichier sum by quarter and area (MFCL)
	string fileSumQA   = param.strdir_output + "SumQArea.txt";
	ofstream ecritSumQA;
	ecritSumQA.open(fileSumQA.c_str(), ios::out);
	
	if (ecritSumQA)
	{
		for (int sp=0; sp<nb_species;sp++)
			ecritSumQA << param.sp_name[sp]<<'\t'<<'\t'<<'\t';
		ecritSumQA <<endl;
		for (int sp=0; sp<nb_species;sp++)
			ecritSumQA << "juvenile: from age 0 to " <<param.age_autonomous[sp]<<'\t'<<'\t'<<'\t';
		ecritSumQA <<endl;
		for (int sp=0; sp<nb_species;sp++)
			ecritSumQA << "young: from age "<< param.age_autonomous[sp]<<" to age "<<param.age_mature[sp]<<'\t'<<'\t'<<'\t';
		ecritSumQA <<endl;
		for (int sp=0; sp<nb_species;sp++)
			ecritSumQA << "Recruit: at age "<< param.age_recruit[sp] <<'\t'<<'\t'<<'\t';
		ecritSumQA <<endl;
		for (int sp=0; sp<nb_species;sp++)
			ecritSumQA << "Adult: from age "<<param.age_mature[sp]<<" to age " <<param.sp_nb_age_class_ad[sp] <<'\t'<<'\t'<<'\t';
		ecritSumQA <<endl;
		for (int sp=0; sp<nb_species;sp++)
			ecritSumQA << "Total biomass: from age "<<param.age_autonomous[sp]<< " to " <<param.sp_nb_age_class_ad[sp] <<'\t'<<'\t'<<'\t';
		ecritSumQA <<endl<<endl;
		ecritSumQA << "year" <<'\t'<<"month" <<'\t';

		if (param.nb_region)
		{
			for (int sp=0; sp<nb_species;sp++)
			{
				if (param.nb_region_sp_B[sp]!=0)
				{
					for (int a=0; a<param.nb_region_sp_B[sp]; a++)
						ecritSumQA<<param.sp_name[sp] << " B Larvae. region " << param.area_sp_B[sp][a] << '\t';
					ecritSumQA<< "Total"<<'\t';
					for (int a=0; a<param.nb_region_sp_B[sp]; a++)
						ecritSumQA<<param.sp_name[sp] << " B Juv. region " << param.area_sp_B[sp][a] << '\t';
					ecritSumQA<< "Total"<<'\t';
					for (int a=0; a<param.nb_region_sp_B[sp]; a++)
						ecritSumQA<<param.sp_name[sp]<< " B Young region " << param.area_sp_B[sp][a] << '\t';
					ecritSumQA<< "Total"<<'\t';
					for (int a=0; a<param.nb_region_sp_B[sp]; a++)
						ecritSumQA<<param.sp_name[sp]<< " B Rec. region " << param.area_sp_B[sp][a] << '\t';
					ecritSumQA<< "Total"<<'\t';
					for (int a=0; a<param.nb_region_sp_B[sp]; a++)
						ecritSumQA<<param.sp_name[sp]<< " B Adult region " << param.area_sp_B[sp][a] << '\t';
					ecritSumQA<< "Total"<<'\t';
					for (int a=0; a<param.nb_region_sp_B[sp]; a++)
						ecritSumQA<<param.sp_name[sp]<< " B tot. region " << param.area_sp_B[sp][a] << '\t';
					ecritSumQA<< "Total"<<'\t';
				}
			}
			for (int sp=0; sp<nb_species;sp++)
			{
				if (param.nb_EEZ!=0)
				{
					for (int a=0; a<param.nb_EEZ; a++)
						ecritSumQA<<param.sp_name[sp]<< "B juv EEZ " << param.EEZ_ID[a] << '\t';
					for (int a=0; a<param.nb_EEZ; a++)
						ecritSumQA<<param.sp_name[sp]<< "B adult EEZ " << param.EEZ_ID[a] << '\t';
					for (int a=0; a<param.nb_EEZ; a++)
						ecritSumQA<<param.sp_name[sp]<< "B total EEZ " << param.EEZ_ID[a] << '\t';
				}
			}
		}
		ecritSumQA<<endl;
		ecritSumQA.close();
	}
	else
	{
		cout<<endl<<"WARNING : Cannot write file "<<fileSumQA<<endl;
	}

	//////////////////////////////////////
	// WRITE HEADERS OF SPATIALCORR.TXT
	// ecriture des noms de variable du fichier SpatialCorr.txt
	// un fichier par espece
	// fichier correlations captures predites-observees
		
	if (nb_fishery)	{
		//Generate the name of the files
		for (int sp=0; sp<nb_species;sp++)
		{
			dymFileSpCorr.push_back(param.strdir_output+param.sp_name[sp]+"_Spatial_Corr.txt");
		}
	}

	ofstream ecritcor;

	if (nb_fishery)	{ 
		for (int sp=0; sp<nb_species;sp++) {  

			ecritcor.open(dymFileSpCorr[sp].c_str(), ios::out);

			if (ecritcor) {
				ecritcor <<"date"<< '\t' ;
				for (int f=0;f<nb_fishery; f++){
					if (param.mask_fishery_sp[sp][f])	
						ecritcor <<"n" << '\t'<<"r " << param.list_fishery_name[f]<<"_"<<param.sp_name[sp]<< '\t'<<"prob" << '\t'
								<< "cpue_r " << param.list_fishery_name[f]<<"_"<<param.sp_name[sp]<< '\t'<<"prob" << '\t';
					
				}
				ecritcor<< endl;
				ecritcor.close();
			}
			else
			{
				cout<<endl<<"WARNING : Cannot write file "<<dymFileSpCorr[sp]<<endl;
			}
		}
	}
    
	//////////////////////////////////////
	//WRITE HEADERS OF SP_LF_Q_FISHERY.TXT
	//ecriture des noms de variable du fichier LF_QFishery.txt
    //fichier Frequences de longueurs par trimestre
	
	if (nb_fishery>0)
	{
		//Generate the name of the files
		for (int sp=0; sp<nb_species;sp++)
		{
			dymFileSpLF.push_back(param.strdir_output+param.sp_name[sp]+"_LF_Q_fishery.txt");
		}
	}

	ofstream ecritQtr_N;

	if (nb_fishery > 0){  
		for (int sp=0; sp<nb_species;sp++){
  
			ecritQtr_N.open(dymFileSpLF[sp].c_str(), ios::out);
			if (ecritQtr_N) {

				ecritQtr_N <<"Quarter "<< '\t'<< "Year "<< endl;
				ecritQtr_N << "length" <<'\t' ;
				for (int reg=0; reg< param.nb_region_sp_B[sp]; reg++){
					for (int f=0; f<nb_fishery; f++){
						if (param.mask_fishery_sp[sp][f])
							ecritQtr_N<<param.list_fishery_name[f]<<"_"<<param.sp_name[sp]<<"_region_"<<param.area_sp_B[sp][reg] <<'\t';
					}
				}
				ecritQtr_N  << endl;
				ecritQtr_N.close();
			}
			else
			{
				cout<<endl<<"WARNING : Cannot write file "<<dymFileSpLF[sp]<<endl;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void CReadWrite::SaveSepodymFileTxt(CParam& param, CMatrices& mat, PMap& map,
				double sumP, DVECTOR& sumF, DVECTOR& sumFprime,
				DVECTOR& sumF_area_pred, DVECTOR& sumF_required_by_sp,
				DVECTOR& mean_omega_sp,int mois2, int yr2, int t_total,
				int ntq, int qtr1, int qtr2, int nbi, int nbj)
{
//cerr << "Force Exit[" << __FILE__ << ':' << __LINE__ << "]: function hasn't been fixed yet\n";
	CSaveTimeArea save;
	CNumfunc fonction;

	const int nb_species = param.get_nbspecies();
	const int nb_fishery = param.get_nbfishery();
	const int nb_forage = param.get_nbforage();

	
	//////////////////////////////////////
	//UPDATE SUMDYM.TXT
	double total=0.f;
	//opening the existing file in mode append
	string fileSumDym	= param.strdir_output +"SumDym.txt";
	ofstream ecritSumDym;
	ecritSumDym.open(fileSumDym.c_str(), ios::app);
	
	if (ecritSumDym)
	{
		// writing the date // Patrick 23Nov04
		ecritSumDym  << mois2 <<"-" << yr2 <<'\t';

		//writing the data
		ecritSumDym << t_total << '\t'<< sumP << '\t' ;
		for (int n=0;n<nb_forage;n++)
		{
			ecritSumDym << sumFprime[n] <<'\t'
				<<sumF[n] <<'\t'
				<< sumF_area_pred[n]<<'\t'
				<<sumF_required_by_sp[n] <<'\t'
				<< mean_omega_sp[n] <<'\t';
		}
		for (int sp=0; sp<nb_species;sp++)
		{
			ecritSumDym << mat.sum_B_juv[sp]  <<'\t'
				<< mat.sum_B_recruit[sp]<<'\t'
				<< mat.sum_B_young[sp]  <<'\t'
				<< mat.sum_B_adult[sp]  <<'\t'
				<< mat.sum_total_pop[sp]<<'\t';
		}
		if (nb_fishery>0){ 
			int k = 0;
			for (int f=0; f<nb_fishery; f++){
				for (int sp=0;sp<nb_species;sp++){
					if (param.mask_fishery_sp[sp][f]){
	  
						// observed fishing effort
						// calcule et enregistre l'effort total de la pecherie
						total=0.f;
						for (int i=1; i< nbi-1; i++){
							for (int j=1; j < nbj-1 ; j++){
								if (map.carte(i,j))
									total += mat.effort[f][i][j];
							}
						}
						ecritSumDym << total << '\t';

				//for (int sp=0; sp< nb_species;sp++){ 
				//	if (param.mask_fishery_sp[sp][f]){
						// observed catch
						total=0.f;
						for (int i=1; i< nbi-1; i++){
							for (int j=1; j < nbj-1 ; j++){
								if (map.carte(i,j))
									total += mat.catch_obs(sp,k,i,j);
							}
						}
						ecritSumDym << total << '\t';
	
						// predicted catch
						total=0.f;
						for (int i=1; i< nbi-1; i++){
							for (int j=1; j < nbj-1 ; j++){
								if (map.carte(i,j))
									total += mat.catch_est(sp,k,i,j);
							}
						}
						ecritSumDym << total << '\t';				
						k++;
					}// else {
					//	ecritSumDym << 0 << '\t' << 0 << '\t';				
					//}
				}
			}
		}
		ecritSumDym << endl;
		ecritSumDym.close();
	}
	else
	{
		cout<<endl<<"WARNING : Cannot write file "<<fileSumDym<<endl;
	}

	//////////////////////////////////////
	//UPDATE SUMQAREA.TXT
	//opening the existing file in mode append
	string fileSumQA   = param.strdir_output + "SumQArea.txt";
	ofstream ecritSumQA;
	ecritSumQA.open(fileSumQA.c_str(), ios::app);
		
	if (ecritSumQA)
	{
		// writing the data
		ecritSumQA << yr2 << '\t' << mois2 << '\t';

		//sum by area (rectangle)
		if (param.nb_region)
		{
			for (int sp=0; sp< nb_species;sp++)
			{
				if (param.nb_region_sp_B[sp]!=0)
				{
					DVECTOR sum_area(0, param.nb_region_sp_B[sp]+1 - 1);

					save.SumByArea(param, map, mat.larvae[sp], sum_area, mat.lat_correction, sp);
					for (int z =0; z< param.nb_region_sp_B[sp]+1;z++)
						ecritSumQA << sum_area[z] <<'\t';

					save.SumByArea(param, map, mat.juvenile[sp], sum_area, mat.lat_correction, sp);
					for (int z =0; z< param.nb_region_sp_B[sp]+1;z++)
						ecritSumQA << sum_area[z] <<'\t';

					save.SumByArea(param, map, mat.young[sp], sum_area, mat.lat_correction, sp);
					for (int z =0; z< param.nb_region_sp_B[sp]+1;z++)
						ecritSumQA << sum_area[z] <<'\t';

					save.SumByArea(param, map, mat.recruit[sp], sum_area, mat.lat_correction, sp);
					for (int z =0; z< param.nb_region_sp_B[sp]+1;z++)
						ecritSumQA << sum_area[z] <<'\t';

					save.SumByArea(param, map, mat.adult[sp], sum_area, mat.lat_correction, sp);
					for (int z =0; z< param.nb_region_sp_B[sp]+1;z++)
						ecritSumQA << sum_area[z] <<'\t';

					save.SumByArea(param, map, mat.total_pop[sp], sum_area, mat.lat_correction, sp);
					for (int z =0; z< param.nb_region_sp_B[sp]+1;z++)
						ecritSumQA << sum_area[z] <<'\t';
				}
			}
		}

		//Sum by EEZ
		DVECTOR sum_EEZ(0, param.nb_EEZ - 1);
		for (int sp=0; sp< nb_species;sp++)
		{
			if (param.nb_EEZ)
			{
				save.SumByEEZ(param, map, nbj-2, nbi-2, mat.juvenile[sp], map.maskEEZ, sum_EEZ);
				for (int z =0; z< param.nb_EEZ;z++)
					ecritSumQA << sum_EEZ[z] <<'\t';

				save.SumByEEZ(param, map, nbj-2, nbi-2, mat.adult[sp], map.maskEEZ, sum_EEZ);
				for (int z =0; z< param.nb_EEZ;z++)
					ecritSumQA << sum_EEZ[z] <<'\t';

				save.SumByEEZ(param, map, nbj-2, nbi-2, mat.total_pop[sp], map.maskEEZ, sum_EEZ);
				for (int z =0; z< param.nb_EEZ;z++)
					ecritSumQA << sum_EEZ[z] <<'\t';
			}
		}
		ecritSumQA<<endl;
		ecritSumQA.close();
	}
	else
	{
		cout<<endl<<"WARNING : Cannot write file "<<fileSumQA<<endl;
	}

	//////////////////////////////////////
	//UPDATE SPATIALCORR.TXT
	ofstream ecritcor;
	if (nb_fishery>0)
	{	
		for (int sp=0;sp<nb_species;sp++)
		{	
			ecritcor.open(dymFileSpCorr[sp].c_str(), ios::app);
			if (ecritcor)
			{
				double cor_catch =0.f;
				double cor_cpue =0.f;
				double z_catch =0.f;
				double z_cpue =0.f;
				double prob_catch= 0.f;
				double prob_cpue= 0.f;
				int nn=0;
				ecritcor << mois2 <<"-" << yr2 <<'\t';
				int k=0;
				for (int f=0; f<nb_fishery; f++)
					if (param.mask_fishery_sp[sp][f]){

					fonction.corcatch(mat.catch_est[sp][k], mat.catch_obs[sp][k], map.imin, map.imax, map.jinf, map.jsup, nn, cor_catch, z_catch, prob_catch, map.carte, 0) ;
					fonction.corcpue(mat.catch_est[sp][k], mat.catch_obs[sp][k], mat.effort[f], map.imin, map.imax, map.jinf, map.jsup, nn, cor_cpue, z_cpue, prob_cpue, map.carte, 0) ;
					ecritcor << nn << '\t'<< cor_catch << '\t' << prob_catch << '\t' << cor_cpue << '\t' << prob_cpue << '\t';
					k++;
				}
				ecritcor << endl;
				ecritcor.close();
			}
			else
			{
				cout<<endl<<"WARNING : Cannot write file "<<dymFileSpCorr[sp]<<endl;
			}
		}
	}
	
	//////////////////////////////////////
	//UPDATE SP_LF_QFISHERY.TXT
	// quarterly catch in number by age(size)by species and fishery
	ofstream ecritQtr_N;
//cout << mat.C_N_sp_age_fishery << endl;
//exit(1);
	// si nouveau trimestre ENREGISTRE a condition qu'il y ait eu un passage (ntq) au minimum
	//if ( (nb_fishery>0) && (qtr2  != qtr1) && (ntq>0) )
	if ( (nb_fishery>0) && (mois2==3 || mois2==6 || mois2==9 || mois2==12)) {
		for (int sp=0; sp< nb_species;sp++)
		{
			ecritQtr_N.open(dymFileSpLF[sp].c_str(), ios::app);
			if (ecritQtr_N)
			{
				ecritQtr_N <<"Quarter "<< qtr1 <<'\t'<< "Year "<< yr2 <<endl;
				for (int a=0;a<param.sp_nb_age_class_ad[sp];a++)
				{
					ecritQtr_N  <<param.length[sp][a]<<'\t';
					for (int reg=0; reg< param.nb_region_sp_B[sp]; reg++)
					{
						for (int k=0;k<param.nb_fishery_by_sp[sp]; k++)
						{
							ecritQtr_N  << mat.C_N_sp_age_fishery_qtr[sp][a][reg][k] <<'\t';
						}
					}
					ecritQtr_N << endl;
				}
				ecritQtr_N.close();
			}
			else
			{
				cout<<endl<<"WARNING : Cannot write file "<<dymFileSpLF[sp]<<endl;
			}
		}
	}
	//////////////////////////////////////
	// Write/Rewrite the file (s)
	//////////////////////////////////////
	//UPDATE SP_LF_Q_SUM.TXT
	//sum of catch in number by age(size) by species, by region and fishery
	//sum for each of four quarter over all the series + sum of the four quarters

	//if ( (nb_fishery>0) && (qtr2  != qtr1) && (ntq>0) )
	if ( (nb_fishery>0) && (mois2==3 || mois2==6 || mois2==9 || mois2==12)) {
  
		for (int sp=0 ; sp<nb_species ; sp++) {

			for (int a=0 ; a<param.sp_nb_age_class_ad[sp] ; a++) {
				
				const int nb_region_sp_B = param.nb_region_sp_B[sp];
				for (int reg=0; reg< nb_region_sp_B; reg++) {
					
					const int nb_fishery_by_sp = param.nb_fishery_by_sp[sp];
					for (int k=0;k<nb_fishery_by_sp; k++) { 

						// somme pour la climatologie trimestrielle par region et pecherie
						mat.Sum_C_N_sp_age_fishery_area[sp][a][qtr1-1][reg][k]
						+= mat.C_N_sp_age_fishery_qtr[sp][a][reg][k];
						// somme pour la climatologie trimestrielle par pecherie
						mat.Sum_C_N_sp_age_fishery_area[sp][a][qtr1-1][nb_region_sp_B][k]
						+= mat.C_N_sp_age_fishery_qtr[sp][a][reg][k];
						// somme pour la climatologie trimestrielle par region
						mat.Sum_C_N_sp_age_fishery_area[sp][a][qtr1-1][reg][nb_fishery_by_sp]
						+= mat.C_N_sp_age_fishery_qtr[sp][a][reg][k];

						// somme totale par pecherie et par region
						mat.Sum_C_N_sp_age_fishery_area[sp][a][5-1][reg][k]
						+= mat.C_N_sp_age_fishery_qtr[sp][a][reg][k];
						// somme totale par pecherie
						mat.Sum_C_N_sp_age_fishery_area[sp][a][5-1][nb_region_sp_B][k]
						+= mat.C_N_sp_age_fishery_qtr[sp][a][reg][k];
						// somme totale par region
						mat.Sum_C_N_sp_age_fishery_area[sp][a][5-1][reg][nb_fishery_by_sp]
						+= mat.C_N_sp_age_fishery_qtr[sp][a][reg][k];

						// somme grand total par trimestre
						mat.Sum_C_N_sp_age_fishery_area[sp][a][qtr1-1][nb_region_sp_B][nb_fishery_by_sp]
						+= mat.C_N_sp_age_fishery_qtr[sp][a][reg][k];

					}
				}
			}
		}
	}

	//////////////////////////////////////
	//WRITE HEADERS OF SP_LF_Q_SUM.TXT
	//ecriture des noms de variable du fichier LF_QFishery.txt
    //fichier Frequences de longueurs par trimestre
	
	//if ( (nb_fishery>0) && (qtr2  != qtr1) && (ntq>0) )
	if ( (nb_fishery>0) && (mois2==3 || mois2==6 || mois2==9 || mois2==12)) {
		//Generate the name of the files
		for (int sp=0; sp<nb_species;sp++) {
	
			dymFileSumSpLF.push_back(param.strdir_output+param.sp_name[sp]+"_LF_Q_sum.txt");
	  	}
	}

	ofstream ecritQtr_S;

	//if ( (nb_fishery>0) && (qtr2  != qtr1) && (ntq>0) ){  
	if ( (nb_fishery>0) && (mois2==3 || mois2==6 || mois2==9 || mois2==12)) {  
		for (int sp=0; sp<nb_species;sp++){  

			ecritQtr_S.open(dymFileSumSpLF[sp].c_str(), ios::out);
			if (ecritQtr_S){

				ecritQtr_S << "length" <<'\t' ;
				for (int reg=0; reg< param.nb_region_sp_B[sp]; reg++){
					for (int f=0; f< nb_fishery; f++){
						if (param.mask_fishery_sp[sp][f])
							ecritQtr_S << param.list_fishery_name[f]<<"_" << param.sp_name[sp]<<"_region_"<<param.area_sp_B[sp][reg]<<'\t';
					}
				}
				for (int f=0; f< nb_fishery; f++){
					if (param.mask_fishery_sp[sp][f])
						ecritQtr_S  <<"sum_"<< param.list_fishery_name[f]<<"_" << param.sp_name[sp]<<'\t' ;
				}

				for (int reg=0; reg< param.nb_region_sp_B[sp]; reg++){

					ecritQtr_S  <<"sum_"<< param.sp_name[sp]<<"_region_" <<param.area_sp_B[sp][reg]<<'\t'  ;
				}
				ecritQtr_S  << endl;
				ecritQtr_S.close();
			}
			else
			{
				cout<<endl<<"WARNING : Cannot write file "<<dymFileSumSpLF[sp]<<endl;
			}
		}

	}
	//////////////////////////////
	// ECRITURE des sommes des captures par age, region pecherie pour les 4 trimestres et le total des 4
	double totalregion; 
	double totalpecherie;
	//if ( (nb_fishery>0) && (qtr2  != qtr1) && (ntq>0) )
	if ( (nb_fishery>0) && (mois2==3 || mois2==6 || mois2==9 || mois2==12)) {	
		for (int sp=0; sp< nb_species;sp++) {	

			ecritQtr_S.open(dymFileSumSpLF[sp].c_str(), ios::app);

			if (ecritQtr_S) {
				for (int q=0; q<5 ;q++) {	

					ecritQtr_S <<"Quarter "<< q+1 <<'\t'<<endl;

					for (int a=0;a<param.sp_nb_age_class_ad[sp];a++) {	

						ecritQtr_S  <<param.length[sp][a]<<'\t';
						totalpecherie=totalregion=0.0;

						const int nb_fishery_by_sp = param.nb_fishery_by_sp[sp];
						const int nb_region_sp_B = param.nb_region_sp_B[sp];
						for (int reg=0; reg< nb_region_sp_B; reg++) {

							for (int k=0; k< nb_fishery_by_sp; k++) { 
								// somme pour la climatologie trimestrielle par region et pecherie
								ecritQtr_S  << mat.Sum_C_N_sp_age_fishery_area[sp][a][q][reg][k] <<'\t'  ;
							}
						}
						for (int k=0; k< nb_fishery_by_sp; k++) {
							// somme pour la climatologie trimestrielle par pecherie
							ecritQtr_S  << mat.Sum_C_N_sp_age_fishery_area[sp][a][q][nb_region_sp_B][k] <<'\t' ;
							totalpecherie+= mat.Sum_C_N_sp_age_fishery_area[sp][a][q][nb_region_sp_B][k];
						}
						for (int reg=0; reg< nb_region_sp_B; reg++) {
							// somme pour la climatologie trimestrielle par region
							ecritQtr_S  <<  mat.Sum_C_N_sp_age_fishery_area[sp][a][q][reg][nb_fishery_by_sp] <<'\t'  ;
							totalregion+= mat.Sum_C_N_sp_age_fishery_area[sp][a][q][reg][nb_fishery_by_sp];
						}
						ecritQtr_S <<totalpecherie <<'\t'  ;
						ecritQtr_S <<totalregion <<'\t'  ;
						ecritQtr_S << endl;
					}
				}	//trimestre suivant
				ecritQtr_S.close();
			} else {
				cout<<endl<<"WARNING : Cannot write file "<<dymFileSumSpLF[sp]<<endl;
			}
		}//espece et fichier suivant
	}
}
*/
