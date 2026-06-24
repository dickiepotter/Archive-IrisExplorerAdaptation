#include <cx/cxParameter.api.h>
#include <cx/cxPyramid.api.h>
#include <cx/DataAccess.h>
#include <cx/PortAccess.h>
#include <cx/Timer.h>
#include <cx/UI.h>

#ifdef __cplusplus
	extern "C" {
#endif

void GLS_main ( void  );
int main_func(int reset, double timeZero, double timeMax,
              int output ,int maxSteps,
              double timestep,double timestep0, double convectX,
              double convectY, double convectZ, int update, int n);
#ifdef __cplusplus
}
#endif

static
void trigger(void *data)
{
  /* Ensure that module is fired again */
  cxFireASAP();
}

static void *tmr = NULL;

void GLS_main ( void  )
{
	static int firsttime = 1;
	static int pauseSim = 1 ;
	int counter = 0;
	static int self_set = 0;
	char label[25];
	int flag = 0;
	long val;
	double tzero,tmax,tstep ;
        static double convx,convy,convz;
        static double minvx,minvy,minvz;
        static double maxvx,maxvy,maxvz;
        static double posx,posy,posz;
	int update,output,msteps;
	int gSize;
        int miliseconds ;
        char *string ;
	double tempConvx, tempConvy, tempConvz ;
        char buffer[2000] ; 
      int paramport;
      cxParameter *interactor;

	if (firsttime) {
    cxInWdgtDisable("convectX") ;
    cxInWdgtDisable("convectY") ;
    cxInWdgtDisable("convectZ") ;
		cxInputDataGet(cxInputPortOpen("timeZero"));
		cxInputDataGet(cxInputPortOpen("timeMax"));
		cxInputDataGet(cxInputPortOpen("output"));
		cxInputDataGet(cxInputPortOpen("maxSteps"));
		cxInputDataGet(cxInputPortOpen("timestep"));
		convx = cxParamDblGet((cxParameter *)cxInputDataGet(cxInputPortOpen("convectX")));
		convy = cxParamDblGet((cxParameter *)cxInputDataGet(cxInputPortOpen("convectY")));
		convz = cxParamDblGet((cxParameter *)cxInputDataGet(cxInputPortOpen("convectZ")));
            (void) cxInWdgtMinMaxGet("convectX", minvx, maxvx);
            (void) cxInWdgtMinMaxGet("convectY", minvy, maxvy);
            (void) cxInWdgtMinMaxGet("convectZ", minvz, maxvz);
            posx = 0.25;
            posy = 0.5;
            posz = 2.0;
		cxInputDataGet(cxInputPortOpen("Run"));
		cxInputDataGet(cxInputPortOpen("update"));
		cxInputDataGet(cxInputPortOpen("Control"));
		cxInputDataGet(cxInputPortOpen("Grid Size"));
		cxInputDataGet(cxInputPortOpen("milisec"));
		cxInWdgtLabelSet("Run","Step -");
		firsttime = 0;

            /* construct parameter string that will set up interactor */
            sprintf(buffer,"%s:vector %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                            cxModuleInstanceNameGet(),
                            posx,posy,posz,minvx,convx,maxvx,minvy,convy,maxvy,minvz,convz,maxvz);
            interactor = cxParamStrNew(buffer);
            paramport = cxOutputPortOpen("Interactor");
            cxOutputDataSet(paramport,interactor);

            sprintf(buffer, "%lf %lf %lf", convx, convy, convz);
            cxInWdgtStrSet("vector",buffer) ;
		cxInputDataGet(cxInputPortOpen("vector"));
		return;
	}

	if (cxInputDataChanged(cxInputPortOpen("Grid Size"))) {
		cxInputDataGet(cxInputPortOpen("Grid Size"));
		flag = 1;
		printf("Frame %d Grid Size\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("timeZero"))) {
		cxInputDataGet(cxInputPortOpen("timeZero"));
		flag = 1;
		printf("Frame %d timeZero\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("timeMax"))) {
		cxInputDataGet(cxInputPortOpen("timeMax"));
		flag = 1;
		printf("Frame %d timeMax\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("output"))) {
		cxInputDataGet(cxInputPortOpen("output"));
		flag = 1;
		printf("Frame %d output\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("maxSteps"))) {
		cxInputDataGet(cxInputPortOpen("maxSteps"));
		flag = 1;
		printf("Frame %d maxSteps\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("timestep"))) {
		cxInputDataGet(cxInputPortOpen("timestep"));
		flag = 1;
		printf("Frame %d timestep\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("convectX"))) {
		cxInputDataGet(cxInputPortOpen("convectX"));
		flag = 1;
		printf("Frame %d convectX\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("convectY"))) {
		cxInputDataGet(cxInputPortOpen("convectY"));
		flag = 1;
		printf("Frame %d convectY\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("convectZ"))) {
		cxInputDataGet(cxInputPortOpen("convectZ"));
		flag = 1;
		printf("Frame %d convectZ\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("update"))) {
		val = cxParamLongGet((cxParameter *)
					cxInputDataGet(cxInputPortOpen("update")));
		if (val < 1)
			cxInWdgtLongSet("update",1);
		flag = 1;
		printf("Frame %d update\n",counter);
	}

	if (flag) {
		printf("Flag Set\n");
		return;
	}

  	
        if (cxInputDataChanged(cxInputPortOpen("vector")))
	{ 
	   string = cxParamStrGet((cxParameter *) cxInputDataGet(cxInputPortOpen("vector"))) ;

           tempConvx = atof(strtok(string," ")) ;
	   tempConvy = atof(strtok(NULL," ")) ;
	   tempConvz = atof(strtok(NULL," ")) ;

	   cxInWdgtDblSet("convectX",tempConvx) ;
	   cxInWdgtDblSet("convectY",tempConvy) ;
	   cxInWdgtDblSet("convectZ",tempConvz) ;

	   if (pauseSim == 1 && (
               cxParamLongGet((cxParameter*)cxInputDataGet(cxInputPortOpen("Control"))) == 2))
           {
	      pauseSim = 0 ;
	      cxInWdgtLongSet("Control",1);
  	      if (tmr != NULL) 
  	      {
		 cxTimerRemove(tmr);
		 tmr = NULL;
	      }
           }
	   else if (cxParamLongGet((cxParameter*)cxInputDataGet(cxInputPortOpen("Control"))) == 1 && pauseSim == 0)
	   {
	      pauseSim = 1 ;
	      cxInWdgtLongSet("Control",2);
	   }
        }


	/* Get control panel values */
	tzero = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("timeZero")));
	tmax = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("timeMax")));
	output = (int)cxParamLongGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("output")));
	msteps = (int)cxParamLongGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("maxSteps")));
	tstep = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("timestep")));
	convx = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("convectX")));
	convy = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("convectY")));
	convz = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("convectZ")));
	update = (int)cxParamLongGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("update")));
	gSize = (int)cxParamLongGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("Grid Size")));

        miliseconds = (int)cxParamLongGet((cxParameter *) cxInputDataGet(cxInputPortOpen("milisec"))) ;


	if (cxParamLongGet((cxParameter*)cxInputDataGet
									(cxInputPortOpen("Control"))) == 2) {
		if (tmr == NULL) {
			tmr = cxTimerAdd(miliseconds, 1, trigger, NULL);
		}

		cxInWdgtDisable("Grid Size");
		counter = main_func(0,tzero,tmax,output,msteps,tstep,
					tstep,convx,convy,convz,update,gSize);
		sprintf(label,"Step %d",counter);
		cxInWdgtLabelSet("Run",label);
		if (counter  < cxParamLongGet((cxParameter *)
							cxInputDataGet(cxInputPortOpen("maxSteps")))) {
			val = cxParamLongGet((cxParameter*)
						cxInputDataGet(cxInputPortOpen("Control")));
			cxInWdgtLongSet("Control",val);
printf("\n\n\nRESET CONTROL %d***************************************** \n\n\n\n",val);
		}
		else {
			cxInWdgtLongSet("Control",1);
			self_set = 1;
		}
	}
	else if (cxParamLongGet((cxParameter*)cxInputDataGet(
							cxInputPortOpen("Control"))) == 1) {
		if (self_set) {
			self_set = 0;
		}
		else {
			cxInWdgtLongSet("Control",1);
			/* Send a reset signal */
			counter = main_func(1,tzero,tmax,output,msteps,tstep,
									tstep,convx,convy,convz,update,gSize);
			self_set = 1;
			if (tmr != NULL) {
				cxTimerRemove(tmr);
				tmr = NULL;
			}
		}
	}
	else if (cxParamLongGet((cxParameter*)cxInputDataGet(cxInputPortOpen("Control"))) == 0) 
        {
		cxInWdgtLongSet("Control",0);
		cxInWdgtEnable("Grid Size");
		cxInWdgtLabelSet("Run","Step -");
		/* Send a reset signal */
		counter = main_func(2,tzero,tmax,output,msteps,tstep,
								tstep,convx,convy,convz,update,gSize);
		self_set = 1;
		if (tmr != NULL) {
			cxTimerRemove(tmr);
			tmr = NULL;
		}

	
	   pauseSim = 1 ;
	}
}

