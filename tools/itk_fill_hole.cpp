/* ----------------------------- MNI Header -----------------------------------
@NAME       : itk_fill_hole
@DESCRIPTION: apply hole filling
@COPYRIGHT  :
              Copyright 2006 Vladimir Fonov, McConnell Brain Imaging Centre, 
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_MINC4ITK
  #include <time_stamp.h>    // for creating minc style history entry
  #include "itkMincImageIOFactory.h"
  #include "itkMincImageIO.h"
  #include "itkMincHelpers.h"
#else
  #include "itk4MincHelpers.h"
#endif

#include <unistd.h>
#include <itkGrayscaleFillholeImageFilter.h>
#include <iostream>
#include <getopt.h>


using namespace  std;
using namespace  minc;

void show_usage (const char *name)
{
  std::cerr 
	  << "Usage: " << name << " <input> <output> " << endl
    << "--clobber clobber output files" << endl
    << "--version" << endl
    << "--fwhm <d>  default 1 "<<endl;
}


int main (int argc, char **argv)
{
#if ( ITK_VERSION_MAJOR < 4 )
  char *_history = time_stamp(argc, argv); 
  std::string history=_history;
  free(_history);
#else
  std::string history= minc_timestamp(argc,argv);  
#endif
  
  int clobber=0;
  int c;
  float fwhm=1;
  
  static struct option long_options[] = { 
    {"clobber", no_argument, &clobber, 1},
    {"version", no_argument, 0, 'v'},
    {"fwhm", required_argument, 0, 'f'},
    {0, 0, 0, 0}};

  for (;;)
  {
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "cvt:r:", long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c)
    {
    case 0:
      break;
    case 'f':
      fwhm = atof(optarg);
      break;
    case 'v':
      cout << "Version: 1.0" << endl;
      return 0;
    case '?':
    default:
      show_usage (argv[0]);
      return 1;
    }
  }

  if((argc - optind)<2)
  {
    show_usage(argv[0]);
    return 1;
  }
  
  std::string input(argv[optind]),output(argv[optind+1]);
  
  if (!clobber && !access (output.c_str (), F_OK))
  {
    cerr << output.c_str () << " Exists!" << endl;
    return 1;
  }
  
  try
  {
    image3d::Pointer img1=load_minc<image3d>(input.c_str());
  
    typedef itk::GrayscaleFillholeImageFilter< image3d, image3d>  FillHoleFilter;
    
    FillHoleFilter::Pointer filter = FillHoleFilter::New();
    
    filter->SetInput( img1 );
    filter->Update();

    minc::image3d::Pointer img_out=  filter->GetOutput();
    minc::append_history(img_out,history);

    save_minc<image3d>(output.c_str(), img_out );
  }
#ifdef HAVE_MINC4ITK
  catch (const minc::generic_error & err) {
    cerr << "Got an error at:" << err.file () << ":" << err.line () << endl;
    return 1;
  }
#endif
  catch( itk::ExceptionObject & err ) 
  { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return 2;
  } 
  return 0;
}
