/*This code produces a lookup table necessary to convert drift times to
  drift distances in the shms drift chambers
*/

#define NPLANES 12
#define TOTAL_BINS 274
void get_LookUp_Values() {

   
  //Read Run Number from txt file
  int run_NUM;
  Long64_t num_evts;        //added
  string input_file;   //added

  TString f0 = "input_RUN.txt";
  ifstream infile(f0);
  infile >> input_file >> run_NUM >> num_evts;
 
  //Open root file containing drift time histos
  TFile *f = new TFile(Form("../root_files/run%d/shms_tzero_corr_histo.root", run_NUM),"READ");
 
  //Define histogram array
  TH1F *h[NPLANES];
 
  //Define the number Drift Chamber planes
  TString plane_names[NPLANES]={"1u1", "1u2", "1x1", "1x2", "1v1", "1v2", "2v2", "2v1", "2x2", "2x1", "2u2", "2u1"};
 
  //Declare bin properties
  int bin_t0[NPLANES];
  int bin_final[NPLANES];					/*Array to store the bin number corresponding to last bin*/
  int bin_Content[NPLANES];          /*Array to store the content (# events) corresponding to the bin with maximum content*/
  double binContent_TOTAL[NPLANES];     /*Array to store sum of all bin contents for each plane*/
  double binSUM[NPLANES];
  int bin;
  int binx;
  double lookup_value[NPLANES];           /*Array to store lookup values for each plane*/ 
  
  //Create an output file to store lookup values  
  ofstream ofs;
  TString lookup_table = Form("../../../PARAM/SHMS/DC/CALIB/pdc_calib_%d.param", run_NUM);
  ofs.open (lookup_table);
 
 
  //Set headers for subsequent columns of data
  ofs << Form("; Lookup Table: RUN %d", run_NUM) << "\n";
  ofs << "; number of bins in time to distance lookup table" << "\n";
  ofs << Form("pdriftbins = %d", TOTAL_BINS+1) << "\n";
  ofs << "; number of 1st bin in table in ns" << "\n";
  ofs << "pdrift1stbin=0" << "\n";
  ofs << "; bin size in ns" << "\n";
  ofs << "pdriftbinsz=1" << "\n";
 
 
 
  //Loop over each plane of shms Drift Chambers (DC1 & DC2)

  for (int ip=0; ip<NPLANES; ip++){
   
    TString drift_time_histo = "pdc"+plane_names[ip]+"_time"; 

    //Get drift time histograms from root file
    h[ip] = (TH1F*)f->Get(drift_time_histo);

    //Get bin corresponding to t0 = 0 ns
    bin_t0[ip] = h[ip]->GetXaxis()->FindBin(0.0);
   
    //Get final bin 
    bin_final[ip] = bin_t0[ip] + TOTAL_BINS;
   
   
   
    //Find total BIN Content over entire integration range
    binContent_TOTAL[ip] = 0; //set counter to zero

    for (bin = bin_t0[ip]; bin <= bin_final[ip]; bin ++ ) {
     
      bin_Content[ip] = h[ip] -> GetBinContent(bin);
     
      binContent_TOTAL[ip] = bin_Content[ip] + binContent_TOTAL[ip];
     
      //   cout << "Bin: " << bin << endl;
      //   cout << "Content " << bin_Content[ip] << endl;
      //   cout << "Content SUM : " << binContent_TOTAL[ip] << endl;
    }
   
    TString headers = "pwc" + plane_names[ip] + "fract=";      
    ofs << headers;	  
   
    //Calculate LookUp Value
   
    binSUM[ip] = 0.0;
    int bin_count = 0;
   
    for (bin = bin_t0[ip]; bin <= bin_final[ip]; bin++) {
     
      bin_Content[ip] = h[ip] -> GetBinContent(bin);
      binSUM[ip] = binSUM[ip] + bin_Content[ip];
     
     
      lookup_value[ip] = binSUM[ip] / binContent_TOTAL[ip];
      bin_count = bin_count + 1;
     
      if (bin_count <= 16 ) {
	ofs << setprecision(5) << lookup_value[ip] << fixed << ",";
      }
     
      else if (bin_count >16 && bin_count <= 274) {
	ofs << setprecision(5) << lookup_value[ip] << ((bin_count+1) % 20 ? "," : "\n") << fixed; 
      }
      else {
	ofs  << setprecision(5) << lookup_value[ip] <<  fixed << endl;	  
      }
     
    }

  }                                            
 
}


	 
  






