#include <cx/cxParameter.api.h>
#include <cx/cxPyramid.api.h>
#include <cx/DataAccess.h>
#include <cx/PortAccess.h>
#include <cx/Timer.h>
#include <cx/Info.h>

#ifdef __cplusplus
	extern "C" {
#endif

void FV_main ( void  );

double main_func(int reset, 
              double timeZero, double timeMax, double CFL,
              double convectX, double convectY, double convectZ,
	      int update, int output, int n,
	      double X1, double Y1, double Z1,
              double X2, double Y2, double Z2 );
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

void FV_main ( void  )
{
	static int firsttime = 1;
	static int state = 1;   /* start in pause/reset mode */
	static int just_started = 0;  /* records if steering has just begun */
	int counter = 0;
	char label[25];
	int flag = 0;
	long val;
	double tzero,tmax,cfl,convx,convy,convz;
	static double last_convx,last_convy,last_convz;
	double minvx,minvy,minvz;
	double maxvx,maxvy,maxvz;
	double posx,posy,posz;
	int update,output;
	int gSize;
	double x1,x2,y1,y2,z1,z2;

	double time;
	char buffer[500] ;
        char *string ;
	int miliseconds ;

	if (firsttime) {
		cxInWdgtDisable("convectX") ;
		cxInWdgtDisable("convectY") ;
		cxInWdgtDisable("convectZ") ;
		cxInWdgtDisable("Control") ;

		cxInputDataGet(cxInputPortOpen("Grid Size"));
		cxInputDataGet(cxInputPortOpen("X1"));
		cxInputDataGet(cxInputPortOpen("X2"));
		cxInputDataGet(cxInputPortOpen("Y1"));
		cxInputDataGet(cxInputPortOpen("Y2"));
		cxInputDataGet(cxInputPortOpen("Z1"));
		cxInputDataGet(cxInputPortOpen("Z2"));
		cxInputDataGet(cxInputPortOpen("timeZero"));
		cxInputDataGet(cxInputPortOpen("timeMax"));
		cxInputDataGet(cxInputPortOpen("CFL"));
		convx = cxParamDblGet((cxParameter *)cxInputDataGet(cxInputPortOpen("convectX")));
		convy = cxParamDblGet((cxParameter *)cxInputDataGet(cxInputPortOpen("convectY")));
		convz = cxParamDblGet((cxParameter *)cxInputDataGet(cxInputPortOpen("convectZ")));
		last_convx = convx;
		last_convy = convy;
		last_convz = convz;
        (void) cxInWdgtMinMaxGet("convectX", &minvx, &maxvx);
        (void) cxInWdgtMinMaxGet("convectY", &minvy, &maxvy);
        (void) cxInWdgtMinMaxGet("convectZ", &minvz, &maxvz);
        posx = 0.25;
        posy = 0.5;
        posz = 2.0;
		cxInputDataGet(cxInputPortOpen("Run"));
		cxInputDataGet(cxInputPortOpen("update"));
		cxInputDataGet(cxInputPortOpen("output"));
		cxInputDataGet(cxInputPortOpen("Control"));
		cxInputDataGet(cxInputPortOpen("milisec"));

        /* construct parameter string that will set up interactor (non-blocking white text type) */
        sprintf(buffer,"%s:vector %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %ld",
                            cxModuleInstanceNameGet(),
                            posx,posy,posz,minvx,convx,maxvx,minvy,convy,maxvy,minvz,convz,maxvz,0);
        cxInWdgtStrSet("interactor", buffer);
		cxInputDataGet(cxInputPortOpen("interactor"));

	    sprintf(buffer,"%lf %lf %lf",convx,convy,convz) ;
        cxInWdgtStrSet("vector",buffer) ;
		cxInputDataGet(cxInputPortOpen("vector"));


		cxInWdgtLabelSet("Run","Step -");


		firsttime = 0;

		return;
	}


	if (cxInputDataChanged(cxInputPortOpen("Grid Size"))) {
		cxInputDataGet(cxInputPortOpen("Grid Size"));
		flag = 1;
		printf("Frame %d Grid Size\n",counter);
	}
	if (cxInputDataChanged(cxInputPortOpen("X1"))) {
		cxInputDataGet(cxInputPortOpen("X1"));
		flag = 1;
		printf("Frame %d X1\n",counter);
	}
	if (cxInputDataChanged(cxInputPortOpen("X2"))) {
		cxInputDataGet(cxInputPortOpen("X2"));
		flag = 1;
		printf("Frame %d X2\n",counter);
	}
	if (cxInputDataChanged(cxInputPortOpen("Y1"))) {
		cxInputDataGet(cxInputPortOpen("Y1"));
		flag = 1;
		printf("Frame %d Y1\n",counter);
	}
	if (cxInputDataChanged(cxInputPortOpen("Y2"))) {
		cxInputDataGet(cxInputPortOpen("Y2"));
		flag = 1;
		printf("Frame %d Y2\n",counter);
	}
	if (cxInputDataChanged(cxInputPortOpen("Z1"))) {
		cxInputDataGet(cxInputPortOpen("Z1"));
		flag = 1;
		printf("Frame %d Z1\n",counter);
	}
	if (cxInputDataChanged(cxInputPortOpen("Z2"))) {
		cxInputDataGet(cxInputPortOpen("Z2"));
		flag = 1;
		printf("Frame %d Z2\n",counter);
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

	if (cxInputDataChanged(cxInputPortOpen("CFL"))) {
		cxInputDataGet(cxInputPortOpen("CFL"));
		flag = 1;
		printf("Frame %d CFL\n",counter);
	}

	if (cxInputDataChanged(cxInputPortOpen("output"))) {
		cxInputDataGet(cxInputPortOpen("output"));
		flag = 1;
		printf("Frame %d output\n",counter);
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


	/* Get control panel values */

    miliseconds = (int)cxParamLongGet((cxParameter *) cxInputDataGet(cxInputPortOpen("milisec"))) ;

	tzero = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("timeZero")));
	tmax = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("timeMax")));
	output = (int)cxParamLongGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("output")));
	cfl = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("CFL")));
	string = cxParamStrGet((cxParameter *) cxInputDataGet(cxInputPortOpen("vector"))) ;

	cxInWdgtDblSet("convectX",atof(strtok(string," "))) ;
	cxInWdgtDblSet("convectY",atof(strtok(NULL," "))) ;
	cxInWdgtDblSet("convectZ",atof(strtok(NULL," "))) ;

	convx = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("convectX")));
	convy = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("convectY")));
	convz = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("convectZ")));

    (void) cxInWdgtMinMaxGet("convectX", &minvx, &maxvx);
    (void) cxInWdgtMinMaxGet("convectY", &minvy, &maxvy);
    (void) cxInWdgtMinMaxGet("convectZ", &minvz, &maxvz);

    posx = 0.25;
    posy = 0.5;
    posz = 2.0;

	if ( cxInputDataChanged(cxInputPortOpen("vector")) ) {
	/* if the user *meant* to send the same values twice, this means a pause (1)*/
	/* if we were running (2) and a run (2) if we were paused or reset (1) */
		if( last_convx == convx &&
			last_convy == convy &&
			last_convz == convz )  {
			
			cxInWdgtLongSet("Control", 3-state);

		}
		else if ( convx == maxvx &&
			convy == maxvy) {
			/* but if the user *meant* to send the max of x and y, define this as a reset (0) */
			/* but restore their earlier, more meaningful values to the sliders */

			cxInWdgtLongSet("Control", 0);

			cxInWdgtDblSet("convectX",last_convx) ;
			cxInWdgtDblSet("convectY",last_convy) ;
			cxInWdgtDblSet("convectZ",last_convz) ;

			convx = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("convectX")));
			convy = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("convectY")));
			convz = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("convectZ")));

			sprintf(buffer,"%lf %lf %lf",convx,convy,convz) ;
			cxInWdgtStrSet("vector",buffer) ;
			cxInputDataGet(cxInputPortOpen("vector"));
		}
	}
	last_convx = convx;
	last_convy = convy;
	last_convz = convz;

	update = (int)cxParamLongGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("update")));
	gSize = (int)cxParamLongGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("Grid Size")));
	x1 = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("X1")));
	x2 = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("X2")));
	y1 = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("Y1")));
	y2 = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("Y2")));
	z1 = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("Z1")));
	z2 = cxParamDblGet((cxParameter *)cxInputDataGet
										(cxInputPortOpen("Z2")));

	if (cxParamLongGet((cxParameter*)cxInputDataGet
									(cxInputPortOpen("Control"))) == 2) {
		state = 2;
		if (tmr == NULL) {
			/* start of steering - immed send a placeholder interactor */
           sprintf(buffer,"%s:vector %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %ld",
                         cxModuleInstanceNameGet(),
                         posx,posy,posz,
                         convx,convx,convx,
                         convy,convy,convy,
                         convz,convz,convz,
						 -1);
            cxInWdgtStrSet("interactor", buffer);
            cxInputDataGet(cxInputPortOpen("interactor"));
			/* unfortunately we have to flush a pyramid to reveal it and only */
			/* an empty one renders quickly enough to beat the user - HW */
			cxOutputDataSet(cxOutputPortOpen("Output"),(void *)cxPyrNew(3));
			cxOutputDataFlush(cxOutputPortOpen("Output"));
			tmr = cxTimerAdd(miliseconds, 1, trigger, NULL);
			just_started = 1;

		}

		cxInWdgtDisable("Grid Size");
		/* unlock the interactor (with results geo send) if only just begun or */
		/* if vector changed (blocking white-red text type) */
        if (just_started || cxInputDataChanged(cxInputPortOpen("vector")))
		{ 
           sprintf(buffer,"%s:vector %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %ld",
                         cxModuleInstanceNameGet(),
                         posx,posy,posz,
                         minvx,convx,maxvx,
                         minvy,convy,maxvy,
                         minvz,convz,maxvz,
						 2);
            cxInWdgtStrSet("interactor", buffer);
            cxInputDataGet(cxInputPortOpen("interactor"));
			just_started = 0;
		}
		time = main_func(0,tzero,tmax,cfl,
			       	    convx,convy,convz,
				    update,output,gSize,
				    x1,y1,z1,x2,y2,z2);
		sprintf(label,"Time %4.3g",time);
		cxInWdgtLabelSet("Run",label);

		if (time  > cxParamDblGet((cxParameter *)
							cxInputDataGet(cxInputPortOpen("timeMax")))) {
			state = 1;
			printf("\n\n\nRESET CONTROL ***************************************** \n\n\n\n");
			cxInWdgtLongSet("Control",1);
			cxInputDataGet(cxInputPortOpen("Control"));

			/* refresh the interactor (non-blocking black text type) */
			sprintf(buffer,"%s:vector %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %ld",
                        cxModuleInstanceNameGet(),
                        posx,posy,posz,
                        minvx,convx,maxvx,
                        minvy,convy,maxvy,
                        minvz,convz,maxvz,
						1);
			cxInWdgtStrSet("interactor", buffer);
			cxInputDataGet(cxInputPortOpen("interactor"));
			/* Send a reset signal */
			time = main_func(1,tzero,tmax,cfl,
			       	            convx,convy,convz,
				            update,output,gSize,
				            x1,y1,z1,x2,y2,z2);

			if (tmr != NULL) {
				cxTimerRemove(tmr);
				tmr = NULL;
			}
		}
	}
	else if (cxParamLongGet((cxParameter*)cxInputDataGet(
							cxInputPortOpen("Control"))) == 1) {
			state = 1;

			/* refresh the interactor if paused following steering (non-blocking black text type) */
			if (tmr != NULL) {

				sprintf(buffer,"%s:vector %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %ld",
                         cxModuleInstanceNameGet(),
                         posx,posy,posz,
                         minvx,convx,maxvx,
                         minvy,convy,maxvy,
                         minvz,convz,maxvz,
						 1);
				cxInWdgtStrSet("interactor", buffer);
				cxInputDataGet(cxInputPortOpen("interactor"));

				/* Send a reset signal */
				time = main_func(1,tzero,tmax,cfl,
			       	            convx,convy,convz,
				            update,output,gSize,
				            x1,y1,z1,x2,y2,z2);

				cxTimerRemove(tmr);
				tmr = NULL;
			}
	}
	else if (cxParamLongGet((cxParameter*)cxInputDataGet(
							cxInputPortOpen("Control"))) == 0) {
			state = 1;
			cxInWdgtEnable("Grid Size");
			cxInWdgtLabelSet("Run","Step - ");

			/* refresh the interactor if newly reset (non-blocking white text type) */
			if (cxInputDataChanged(cxInputPortOpen("Control"))) {
				sprintf(buffer,"%s:vector %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %ld",
                         cxModuleInstanceNameGet(),
                         posx,posy,posz,
                         minvx,convx,maxvx,
                         minvy,convy,maxvy,
                         minvz,convz,maxvz,
						 0);
				cxInWdgtStrSet("interactor", buffer);
				cxInputDataGet(cxInputPortOpen("interactor"));

				/* Send a reset signal */
				time = main_func(2,tzero,tmax,cfl,
			       	            convx,convy,convz,
				            update,output,gSize,
				            x1,y1,z1,x2,y2,z2);
			}

			if (tmr != NULL) {
				cxTimerRemove(tmr);
				tmr = NULL;
			}

	}
}

