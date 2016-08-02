#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <TGraph.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TRandom.h>
#include "DrawDetector.hh"
#include "WireposEP.hh"
#include "NeuralNet.hh"
#include "Density_Cut.hh"
#include <TH1.h>
#include "LayerInf140104.hh"

using namespace std;
int main(int argc, char** argv){
        
  //gROOT->Reset();

    //gROOT->SetBatch();
    //gStyle->SetPadGridX(1);
    //gStyle->SetPadGridY(1);
    gRandom->SetSeed( time(NULL) );
    
    TApplication app("app", &argc, argv);

   
    //    TFile* file = new TFile("/Users/hayashi_oki/Workspace/Track-finding/Helix/NN/Data/run792_noise_10%.root");
    TFile* file = new TFile("/Users/hayashi_oki/Workspace/Track-finding/Helix/NN/Data/run792_noise_10%.root");
    TTree* t = (TTree*)file->Get("tree_noise");

    //TCanvas* c1 = new TCanvas("c1","c1",10,10,800,800);               
    //TCanvas* c2 = new TCanvas("c2","c2",10,10,800,800);
    //TCanvas* c3 = new TCanvas("c3","c3",1000,10,800,800); 
    //TCanvas* c4 = new TCanvas("c4","c4",10,500,800,800);  
    //TCanvas* c5 = new TCanvas("c5","c5",100,10,800,800);  
    //TCanvas* c6 = new TCanvas("c6","c6",10,50,800,800);   

    //TH1D* h1 = new TH1D("h1","h1",50,0,100);
    
    int NOfHit;
    vector<int>* ilayer = 0;
    vector<int>* icell = 0;
    vector<int>* type = 0;
    vector<double>* minhit_x = 0;
    vector<double>* minhit_y = 0;
    vector<double>* minhit_z = 0;

    t->SetBranchAddress("nwirehit",&NOfHit);
    t->SetBranchAddress("ilayer",&ilayer);
    t->SetBranchAddress("icell",&icell);
    t->SetBranchAddress("minhit_x",&minhit_x);
    t->SetBranchAddress("minhit_y",&minhit_y);
    t->SetBranchAddress("minhit_z",&minhit_z);
    t->SetBranchAddress("type",&type);
    
    unsigned int count_efficiency = 0; 
    //unsigned int count_efficiency_40 = 0;

    //for(int a=0;a<1;a++){                            //--- if want to check each event display, use this for statement
    


    for(int a=0;a<t->GetEntries();a++){

	cout << "*****  a = " << a << "  *****" << endl;

	t->GetEntry(a);

	//---check events if the signal exist or not
        int check_event = 0;
	
	for(int b=0;b<NOfHit;b++){
	    if(type->at(b) == 1){
		check_event++;
	    }
	}
	if(check_event<10){
	    cout << "---This event do not have any signals---" << endl;
	}
	if(check_event>=10){
	    
            //--- first cut (search crossing hit wires in adjacent layer)---------------------------------------------------//
            
	    vector<double> x_EP;	       //--* x-coordinate of hit wire                                                     
	    vector<double> y_EP;               //--* y-coordinate of hit wire                                                     
	    vector<double> z_EP;               //--* z-coordinate of hit wire(not use now)                                                     
	    vector<double> x_EP2;              //--* x-coordinate of hit wire that path through crossing cut 
	    vector<double> y_EP2;              //--* y-coordinate of hit wire that path through crossing cut 
	    vector<double> z_EP2;              //--* z-coordinate of hit wire that path through crossing cut(not use now) 
	    vector<double> x_sig;              //--* x-coordinate of hit wire by signal 
	    vector<double> y_sig;              //--* y-coordinate of hit wire by signal 
	    vector<double> x_bg;               //--* x-coordinate of hit wire by noise 
	    vector<double> y_bg;               //--* x-coordinate of hit wire by noise 
	    double x_ep, y_ep, z_ep, x_ep2, y_ep2, z_ep2, x_ep3, y_ep3, z_ep3, x_ep4, y_ep4, z_ep4; 
	    
	    int count = 0;  
            int count2 = 0; 
            int index = 0;  
	    int count5 = 0;
	    
	    int N[18] = {};  
	    
            cout << "NOfHit = " << NOfHit << endl;

	    for(int i=0;i<NOfHit;i++){
		N[ilayer->at(i)-1]++;
	    }

	    //--- layers 
	    for(int i=0;i<18;i++){
		if(i>0){
		    index += N[i-1];

		    int NOfHit_L = N[i-1];                
                    double check[1000] = {};    //--- check double count for xEP, yEP
                    double check2[1000] = {};   //--- check double count for xEP2, yEP2
		   
		    //--- cells (upper)
		    for(int j=0;j<N[i];j++){
			WireposEP(ilayer->at(index+j), icell->at(index+j), &x_ep, &y_ep, &z_ep);
			
			if(i == 17){
			    x_EP.push_back(x_ep);
			    y_EP.push_back(y_ep);
			}
			if(type->at(index+j) == 0){
			    x_bg.push_back(x_ep);
			    y_bg.push_back(y_ep);
			}
			if(type->at(index+j) == 1){
			    x_sig.push_back(x_ep);
			    y_sig.push_back(y_ep);
			}

			LayerInf layerinf1(ilayer->at(index+j));
			double theta_up = layerinf1.GetInterval() * icell->at(index+j);

			//--- cells (lower)
			for(int k=0;k<N[i-1];k++){
			    WireposEP(ilayer->at(index-N[i-1]+k), icell->at(index-N[i-1]+k), &x_ep3, &y_ep3, &z_ep3);

			    if(check[k] == 0){
			       x_EP.push_back(x_ep3);  
			       y_EP.push_back(y_ep3); 
			       check[k] = 1;
			    }
			    
			    if(i == 1 && j == 0){
				if(type->at(index-N[i-1]+k) == 0){
				    x_bg.push_back(x_ep3);
				    y_bg.push_back(y_ep3);
				}
				if(type->at(index-N[i-1]+k) == 1){
				    x_sig.push_back(x_ep3);
				    y_sig.push_back(y_ep3);
				}
			    }

			    int flag = 0;
			    if(icell->at(index-N[i-1]+k)+1 == icell->at(index-N[i-1]+k+1)){
				flag = 1;
				count2++;
			    }
			    if(flag == 0){
				count2 = 0;
			    }

			    LayerInf layerinf2(ilayer->at(index-N[i-1]+k));
			    double theta_down = layerinf2.GetInterval() * icell->at(index-N[i-1]+k);

			    //--- Compare theta_up and theta_down
			    if((i%2 != 0 && theta_up-theta_down>=-0.3 && theta_up-theta_down<0) || (i%2 == 0 && theta_up-theta_down>0 && theta_up-theta_down<=0.3)){
				if(check2[k] == 0){                                                                  
				    x_EP2.push_back(x_ep3);
				    y_EP2.push_back(y_ep3);
				}if(check2[k] == 1){
				    continue;
				}
				check2[k] = 1;
			    }
			    
			    //--- look for hits in a line continuously in the same layer
			    if(count2>3 && check2[k] == 0){
				for(int m=0;m<count2;m++){
				    if(check2[k-m] == 0){
					x_EP2.push_back(x_EP[index-N[i-1]+k-m]); 
					y_EP2.push_back(y_EP[index-N[i-1]+k-m]); 
					check2[k-m] = 1;
				    }	
				}       
			    }
			}
			count = 0;
		    
		        //--- look for hits in a line continuously in the same layer in only the last 18th layer
			//--- because above for(k) statement cannot look the last layer 
			if(i == 17){
                            int flag = 0;                                                   

			    if((icell->at(index+j)-1 == icell->at(index+j-1)) || (icell->at(index+j-1) == icell->at(index+j))){ 
				flag = 1;                                                   
                                count5++;                                                   
                            }                                                               
                            if(flag == 0){                                                  
                                count5 = 0;                                                 
                            }                                                               
			    if(count5>3){                       
                                for(int m=0;m<count5;m++){                        
				    if(check2[j-1] == 0){
					x_EP2.push_back(x_EP[index+j-1-m]);  
					y_EP2.push_back(y_EP[index+j-1-m]);

					check2[j-1] = 1;
				    }
                                }                                                 
                            }                                                     
			}
		    }
		}
	    }
	    
            //--------------------------------------------------------------------------------------------------------------//

	    //--- apply neural network and density cut ---------------------------------------------------------------------//
	    
	    vector<double> signalNN_X;         //--* x-coordinate of hits selected by 1st neural network 
	    vector<double> signalNN_Y;         //--* y-coordinate of hits selected by 1st neural network 
	    vector<double> signalNN_X_2;       //--* x-coordinate of hits selected by 2nd neural network(not use now)  
	    vector<double> signalNN_Y_2;       //--* y-coordinate of hits selected by 2nd neural network(not use now)  
	   
            //---NN Parameter is {NOfStep, lambda, a, b, alpha, beta, C, T, V_ij_threshold, distance_cut, angle_cut}
            NNParameter param1 = {500000, 5., 1., 1., 5., 0., 100., 1., 0.8, 7., 0.1};
            NeuralNet(&x_EP2, &y_EP2, &signalNN_X, &signalNN_Y, &param1);
            
            vector<double> signalNN_X_cut;     //--* x-coordinate of hits path through the density cut
            vector<double> signalNN_Y_cut;     //--* y-coordinate of hits path through the density cut 
	    
	    //--- 1st density cut
	    if(signalNN_X.size() > 120){
		Density_Cut(&signalNN_X, &signalNN_Y, &signalNN_X_cut, &signalNN_Y_cut, 20., 15);
	    }else{
		//--- if number of hits is not so large, density cut will be not applied
		cout << "No more cuts apply" << endl; 
		signalNN_X_cut = signalNN_X;
		signalNN_Y_cut = signalNN_Y;
	    }

	    //--- 2nd neural net(not use now)
	    //NNParameter param2 = {50000, 15., 1.0, 1.0, 15., 0.1, 10., 1.0, 0.8, 50., 0.1};
	    //NeuralNet(&signalNN_X_cut, &signalNN_Y_cut, &signalNN_X_2, &signalNN_Y_2, &param2);

            vector<double> signalNN_X_2_cut;   //--* x-coordinate of hits path through the 2nd density cut  
            vector<double> signalNN_Y_2_cut;   //--* y-coordinate of hits path through the 2nd density cut  
	    
	    //--- 2nd density cut
	    if(signalNN_X_cut.size() > 120){
		Density_Cut(&signalNN_X_cut,&signalNN_Y_cut, &signalNN_X_2_cut, &signalNN_Y_2_cut, 40., 30);
	    }else{
		//--- if number of hits is not so large, density cut will be not applied 
		cout << "No more cut apply" << endl;
		signalNN_X_2_cut = signalNN_X_cut;
		signalNN_Y_2_cut = signalNN_Y_cut;
	    }

	    //--------------------------------------------------------------------------------------------------------------//
	    
	    //--- count signal and noise hits to calculate efficiency ------------------------------------------------------//

	    double count3 = 0.;
	    double count3_2 = 0.;
            double count3_3 = 0.;

	    cout << "signalsize is " << x_sig.size() << endl;
	    
	    for(int i=0;i<x_sig.size();i++){                                            
	        for(int j=0;j<signalNN_X.size();j++){                               
                    if(x_sig[i] == signalNN_X[j] && y_sig[i] == signalNN_Y[j]){ 
                	count3_3++;                                                       
                    }                                                                   
	        }                                                                       
	    }                                                                           
	    
	    for(int i=0;i<x_sig.size();i++){
		for(int j=0;j<signalNN_X_cut.size();j++){
		    if(x_sig[i] == signalNN_X_cut[j] && y_sig[i] == signalNN_Y_cut[j]){
			count3++;
		    }
		}
	    }
	    
	    for(int i=0;i<x_sig.size();i++){                                             
	        for(int j=0;j<signalNN_X_2_cut.size();j++){                                    
	            if(x_sig[i] == signalNN_X_2_cut[j] && y_sig[i] == signalNN_Y_2_cut[j]){          
	        	count3_2++;                                                        
	            }                                                                    
	        }                                                                        
	    }                                                                            

            double count4 = signalNN_X_cut.size() - count3;
	    double count4_2 = signalNN_X_2_cut.size() - count3_2;
	    double count4_3 = signalNN_X.size() - count3_3;
	    
	    double efficiency = (count3/double(x_sig.size()))*100.; 
            double efficiency_2 = (count3_2/x_sig.size())*100.;
            double efficiency_3 = (count3_3/x_sig.size())*100.;
	    double noise_reduction = 100. - (count4/x_bg.size())*100.;
            double noise_reduction_2 = 100. - (count4_2/x_bg.size())*100.;
            double noise_reduction_3 = 100. - (count4_3/x_bg.size())*100.;
	    
	    
	    cout << "-------" << endl;
	    cout << "After searching crossing point, NOfHit: " << x_EP2.size() << endl;
	    cout << "Noise reductoin rate is " << 100. - (x_EP2.size()/NOfHit)*100. << endl;
	    cout << "-------" << endl;
	    
            cout << "-------" << endl;
            cout << "After NN, Efficiency is:" << efficiency_3 << " [%] " << endl;
            cout << "NOfhit is " << signalNN_X.size() << endl;
	    cout << "Noise reduction rate is " << noise_reduction_3 << " [%] " << endl;
	    cout << "-------" << endl;

	    cout << "--------" << endl; 
	    cout << "In 1st NN after cut, Efficiency is:" << efficiency << " [%] " << endl; 
	    cout << "Noise reduction rate is " << noise_reduction << " [%] " << endl;
	    cout << "--------" << endl;

            cout << "--------" << endl;                                             
            cout << "In 2nd cut, Efficiency is:" << efficiency_2 << " [%] " << endl;  
            cout << "Noise reduction rate is " << noise_reduction_2 << " [%] " << endl;                                  
            cout << "--------" << endl;                                            

	    //--- modify the conditions of 'success'
	    if(efficiency_2>=50. && (count3_2/count4_2)>=1.){
	        count_efficiency++;
	    }

            cout << endl;
            cout << "Finally," << endl;
            cout << "number of events 80 [%] over :" << count_efficiency << endl;
            cout << "signal vs noise = " << count3_2 << ":" << count4_2 << endl;


            cout << "======================================" << endl;
	



	    //h1->Fill(efficiency_2);
	    
            //--------------------------------------------------------------------------------------------------------------// 
	    
	    //--- clear vectors
	    ilayer->clear();      
	    icell->clear();       
	    type->clear();        
	    minhit_x->clear(); 
	    minhit_y->clear(); 
	    minhit_z->clear(); 
	    x_EP.clear(); 
            y_EP.clear(); 
            z_EP.clear(); 
            x_EP2.clear();
            y_EP2.clear();
            z_EP2.clear();
            x_sig.clear();
            y_sig.clear();
            x_bg.clear(); 
            y_bg.clear(); 
            signalNN_X.clear(); 
            signalNN_Y.clear(); 
            signalNN_X_2.clear();
            signalNN_Y_2.clear();
            signalNN_X_cut.clear();
	    signalNN_Y_cut.clear();
            signalNN_X_2_cut.clear();
            signalNN_Y_2_cut.clear();

            //--- drawing --------------------------------------------------------------------------------------------------// 
	    
	    //c2->cd();                                                                        
            ////TGraph* gsigNN = new TGraph(signalNN_X.size(), &signalNN_X[0], &signalNN_Y[0]);  
            //TGraph* gsigNN = new TGraph(x_EP2.size(), &x_EP2[0], &y_EP2[0]);   
	    //gsigNN->Draw("ap");                                                          
            //gsigNN->SetMarkerStyle(4);                                                       
            //gsigNN->SetMarkerSize(0.5);                                                      
            //gsigNN->SetMarkerColor(4);                                                       
	    ////gsigNN->SetTitle("after 1st NN");
            //gsigNN->SetTitle(0);
	    //gsigNN->GetXaxis()->SetTitle("X [cm]");
	    //gsigNN->GetYaxis()->SetTitle("Y [cm]");
	    //gsigNN->SetMaximum(90);
	    //gsigNN->SetMinimum(-90);
	    //gsigNN->GetXaxis()->SetLimits(-90,90);

	    //DrawDetector();                                                                  
	    //
	    //c3->cd();                                                       
	    ////TGraph* gcut = new TGraph(x_EP2.size(), &x_EP2[0], &y_EP2[0]);  
	    ////gcut->Draw("p,same");                                           
	    ////gcut->SetMarkerStyle(4);                                        
	    ////gcut->SetMarkerSize(0.5);                                       
	    ////gcut->SetMarkerColor(4);                                        
	
	    //TGraph* gsig = new TGraph(x_sig.size(), &x_sig[0], &y_sig[0]);  
	    //gsig->Draw("ap");                                           
	    //gsig->SetMarkerStyle(4);                                        
	    //gsig->SetMarkerSize(0.5);                                       
	    //gsig->SetMarkerColor(6);                                        
	    ////gsig->SetTitle("noise 9%");
	    //gsig->SetTitle(0);
	    //gsig->GetXaxis()->SetTitle("X [cm]"); 
	    //gsig->GetYaxis()->SetTitle("Y [cm]"); 
	    //gsig->SetMaximum(90);                
            //gsig->SetMinimum(-90);               
            //gsig->GetXaxis()->SetLimits(-90,90); 

	    //TGraph* gbg = new TGraph(x_bg.size(), &x_bg[0], &y_bg[0]);      
	    //gbg->Draw("p,same");                                            
	    //gbg->SetMarkerStyle(4);                                         
	    //gbg->SetMarkerSize(0.5);                                        
	    //gbg->SetMarkerColor(4);                                         
	    //DrawDetector();                                                 


	    //c4->cd();
	    //TGraph* gsigNN2 = new TGraph(signalNN_X.size(), &signalNN_X[0], &signalNN_Y[0]);   
	    //gsigNN2->Draw("ap");                                                                 
	    //gsigNN2->SetMarkerStyle(4);                                                              
	    //gsigNN2->SetMarkerSize(0.5);                                                             
	    //gsigNN2->SetMarkerColor(4);                                                              
	    //gsigNN2->SetTitle("after NN");
	    //gsigNN2->GetXaxis()->SetTitle("X [cm]");  
	    //gsigNN2->GetYaxis()->SetTitle("Y [cm]");  
	    //gsigNN2->SetMaximum(90);                 
	    //gsigNN2->SetMinimum(-90);                
	    //gsigNN2->GetXaxis()->SetLimits(-90,90);  
	    //DrawDetector();


	    //c5->cd();                                                                                 
            //TGraph* gsigNN_cut = new TGraph(signalNN_X_cut.size(), &signalNN_X_cut[0], &signalNN_Y_cut[0]);    
            //gsigNN_cut->Draw("ap");                                                                  
            //gsigNN_cut->SetMarkerStyle(4);                                                               
            //gsigNN_cut->SetMarkerSize(0.5);                                                              
            //gsigNN_cut->SetMarkerColor(4);                                                               
            ////gsigNN_cut->SetTitle("after 1st NN cut");
            //gsigNN_cut->SetTitle(0);
	    //gsigNN_cut->GetXaxis()->SetTitle("X [cm]");  
	    //gsigNN_cut->GetYaxis()->SetTitle("Y [cm]");  
	    //gsigNN_cut->SetMaximum(90);                  
	    //gsigNN_cut->SetMinimum(-90);                 
	    //gsigNN_cut->GetXaxis()->SetLimits(-90,90);   
	    //DrawDetector();                                                                           
            //
	    //c6->cd();                                                                                 
            //TGraph* gsigNN2_cut = new TGraph(signalNN_X_2_cut.size(), &signalNN_X_2_cut[0], &signalNN_Y_2_cut[0]);    
            //gsigNN2_cut->Draw("ap");                                                                  
            //gsigNN2_cut->SetMarkerStyle(4);                                                               
            //gsigNN2_cut->SetMarkerSize(0.5);                                                              
	    //gsigNN2_cut->SetMarkerColor(4);                                                               
	    ////gsigNN2_cut->SetTitle("after 2nd NN cut: Result");
            //gsigNN2_cut->SetTitle(0);
	    //gsigNN2_cut->GetXaxis()->SetTitle("X [cm]");  
	    //gsigNN2_cut->GetYaxis()->SetTitle("Y [cm]");  
	    //gsigNN2_cut->SetMaximum(90);                  
	    //gsigNN2_cut->SetMinimum(-90);                 
	    //gsigNN2_cut->GetXaxis()->SetLimits(-90,90);   
	    //DrawDetector();                                                                           

	}
    }
    
    //c1->cd();
    //h1->Draw();
    //h1->SetFillStyle(1001);
    //h1->SetFillColor(6);
    //h1->SetLineColor(6);
    //h1->SetStats(0);
    //h1->SetTitle(0);
    //h1->GetXaxis()->SetTitle("finding efficiency [%]");

    //c1->Update();
    //c2->Update();
    //c3->Update();
    //c4->Update();
    //c5->Update();
    //c6->Update();
    
    //app.Run();
}



    
