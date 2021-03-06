
/*=========================================================================

  Program: GDCM (Grassroots DICOM). A DICOM library

  Copyright (c) 2006-2011 Mathieu Malaterre
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*
 * Dummy implementation of C.7.1.3 Clinical Trial Subject Module
 *
 * Usage:
 *  ClinicalTrialAnnotateTest gdcmData/012345.002.050.dcm out.dcm
 */

#include "gdcmReader.h"
#include "gdcmWriter.h"
#include "gdcmAnonymizer.h"

#include "gdcmUIDGenerator.h"
#include "gdcmFile.h"
#include "gdcmTag.h"
#include "gdcmSystem.h"
#include "gdcmAttribute.h"
#include "gdcmStringFilter.h"

#include <stdio.h>
#include <stdlib.h>

using namespace std;

void help(char* exec){
    cerr<<"usage: "<<exec<<" -dcm <dicom file name> -outdcm <output dcm filename>"<<endl;
    cerr<<"options: "<<endl;
    cerr<<"-fileIds <cvs formated filename with 'patientId, anonymizeId' The patientId is replaced with specific anonymizeId>"<<endl;
    cerr<<"-pid <patient pId>"<<endl;
}

int main(int argv, char **argc)
{

    string anonymizepId = "";
    string anonymizeIdsFilename = "";
    string dcmFile = "";
    string dcmoutFile = "";


    for(int i = 1; i < argv; i++){
        string input = string(argc[i]);

        if(input.compare("-idfile")==0){
            
            anonymizeIdsFilename = string(argc[i+1]);
        }else if(input.compare("-pid")==0){
            
            anonymizepId = string(argc[i+1]);
        }else if(input.compare("-dcm")==0){
            
            dcmFile = string(argc[i+1]);
        }else if(input.compare("-outdcm")==0){
            
            dcmoutFile = string(argc[i+1]);


        }else if(input.compare("-help")==0 || input.compare("--help")==0){
            help(argc[0]);
            return 0;
        }

    }

	cout<<anonymizeIdsFilename<<endl;
    vector< string> elems;
    std::ifstream  data(anonymizeIdsFilename.c_str());

    std::string line;
    typedef map<int, int> mapids;
    mapids ids;
    //while(std::getline(data,line, '\r'))
    while(std::getline(data,line))
    {
        std::stringstream  lineStream(line);
        std::string        cell;
        int i = 0;
        string patientId, mrn;
        while(std::getline(lineStream,cell,','))
        {
            if(i == 0){
                patientId = cell.c_str();
            }else if(i == 1){
                mrn = cell.c_str();
            }
            i++;
            elems.push_back(cell);
        }
        ids.insert(pair<int, int>(atoi(mrn.c_str()), atoi(patientId.c_str())));
    }

	data.close();

  string filename = dcmFile;

  if(filename.find("(")!=string::npos){
      filename.replace(filename.find("("), filename.find("(") + 1, "\(");
  }
  if(filename.find(")")!=string::npos){
      filename.replace(filename.find(")"), filename.find(")") + 1, "\)");
  }

  string imagedir(filename) ;
  imagedir = imagedir.substr(0, imagedir.find_last_of("/"));
  imagedir = imagedir.substr(imagedir.find_last_of("/") + 1);


  cout<<"Number of ids= "<<ids.size()<<endl;
    cout<<"input= "<< filename<<endl;


  string outfilename = dcmoutFile;


  gdcm::Reader reader;
  reader.SetFileName( filename.c_str() );
  if( !reader.Read() )
    {
    std::cerr << "Could not read: " << filename << std::endl;
    return 1;
    }

  // The output of gdcm::Reader is a gdcm::File
  /*gdcm::File &file = reader.GetFile();

  // the dataset is the the set of element we are interested in:
  gdcm::DataSet &ds = file.GetDataSet();
  //gdcm::Tag tsis(0x0008,0x2112); // SourceImageSequence
  gdcm::Tag tbday(0x0010,0x0030);
  gdcm::DataElement bday;
  if ( ds.FindDataElement( tbday ) ){
       bday.SetValue(ds.GetDataElement( tbday ).GetValue());
  }*/

  gdcm::Anonymizer ano;
  ano.SetFile( reader.GetFile() );

  //ano.RemoveGroupLength();
  //ano.RemovePrivateTags();

  gdcm::MediaStorage ms = gdcm::MediaStorage::MRImageStorage;

    gdcm::UIDGenerator gen;
    ano.Replace( gdcm::Tag(0x0008,0x16), ms.GetString() );
    //std::cout << ms.GetString() << std::endl;
    ano.Replace( gdcm::Tag(0x0008,0x18), gen.Generate() );

    gdcm::StringFilter sf;
    sf.SetFile(reader.GetFile());
    int patientMRN = atoi(sf.ToString(gdcm::Tag(0x0010,0x20)).c_str());

    if(ids.size() > 0 && ids.find(patientMRN) != ids.end()){

        int sessionId = ids[patientMRN];

        cout<<"Id found: "<<sessionId<<endl;
        char buff[50];
        sprintf(buff, "%d", sessionId);
        ano.Replace( gdcm::Tag(0x0010,0x20), buff );


    }else if(anonymizepId.compare("") != 0){
        int anonymid = atoi(anonymizepId.c_str());

        char buff[50];
        sprintf(buff, "%d", anonymid);
        ano.Replace( gdcm::Tag(0x0010,0x20), buff  );
    }else{

        return 0;
        cout<<"Generating UUID as patientID."<<endl;

        const char* genId = gen.Generate();
        ano.Replace( gdcm::Tag(0x0010,0x20), genId );
    }


    ano.Replace( gdcm::Tag(0x0010,0x10), "Anonim" );
    ano.Replace( gdcm::Tag(0x0002,0x0010), "1.2.840.10008.1.2.1" );//Compression

    ano.Empty( gdcm::Tag(0x0010,0x30) );//birth day
    ano.Empty( gdcm::Tag(0x0010,0x40) );//sex
    ano.Empty( gdcm::Tag(0x0008,0x90) );//Refering physician name
    ano.Empty( gdcm::Tag(0x0020,0x10) );//study id
    ano.Empty( gdcm::Tag(0x0020,0x11) );//Series number
    ano.Empty( gdcm::Tag(0x0008,0x50) );//Accesion
    ano.Empty( gdcm::Tag(0x0010,0x4000) );//Patient comments
    ano.Empty( gdcm::Tag(0x0032,0x4000) );//Study comments


    string strtag = sf.ToString(gdcm::Tag(0x0008, 0x103e));
    if(strtag.find("screen save") != string::npos || strtag.find("Screen Save") != string::npos){

            cout<<"Screen capture, image not saved. "<<endl;

        return 1;
    }else{

        if(outfilename.find_last_of("/") != string::npos){
            string outdir = outfilename.substr(0, outfilename.find_last_of("/"));
            string exec = "mkdir -p " + outdir;
            system(exec.c_str());
        }


            cout<<"output anonymized: "<< outfilename<<endl;


      gdcm::Writer writer;
      writer.SetFile( reader.GetFile() );
      writer.SetFileName( outfilename.c_str() );


      if( !writer.Write() )
        {
        return 1;
        }

    }

  return 0;
}

