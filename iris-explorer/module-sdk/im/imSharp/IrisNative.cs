using System;
using System.Text;
using System.Runtime.InteropServices;

namespace IrisSharp
{
    public class IrisNative
    {
        [DllImport("mcw50.dll")]
        public static extern int cxInputPortCount();

        [DllImport("mcw50.dll")]
        public static extern int cxOutputPortCount();

        [DllImport("mcw50.dll")]
        public static extern IntPtr cxInputPortNames();

        [DllImport("mcw50.dll")]
        public static extern IntPtr cxOutputPortNames();

        [DllImport("mcw50.dll")]
        public static extern int cxInputPortOpen( [MarshalAs(UnmanagedType.LPStr)] string portName );

        [DllImport("mcw50.dll")]
        public static extern int cxOutputPortOpen( [MarshalAs(UnmanagedType.LPStr)] string portName );

        [DllImport("mcw50.dll")]
        public static extern IntPtr cxInputTypeGet(int portId );

        [DllImport("mcw50.dll")]
        public static extern IntPtr cxOutputTypeGet( int portId );

        [DllImport("mcw50.dll")]
        public static extern int cxInputConnectsGet(int portId);

        [DllImport("mcw50.dll")]
        public static extern int cxOutputConnectsGet(int portId);

        [DllImport("mcw50.dll")]
        public static extern IntPtr cxModuleNameGet();

        [DllImport("mcw50.dll")]
        public static extern IntPtr cxModuleInstanceNameGet();

        [DllImport("mcw50.dll")]
        public static extern IntPtr cxModuleBaseNameGet();

        [DllImport("mcw50.dll")]
        public static extern int cxInputDataConnIDGet( int port, ref IntPtr connIDs );


     //   [DllImport("mcw50.dll")]
     //   public static extern void	cxOutputDataSet( int portIndex, cxobject *data );

        /*
        [DllImport("mcw50.dll")]
        public static extern void cxEnterUserCode(int count, params object[] parameters);
        
        [DllImport("mcw50.dll")]
        public static extern void cxDestroyFuncPost( IntPtr val );
        
        [DllImport("mcw50.dll")]
        public static extern void cxDestroyFuncExec();
        


        [DllImport("mcw50.dll")]
        public static extern void callHookFunction(cxHookType type, string name, ref int link);
    }

    [Flags]
    public enum cxHookType
    {
        cx_func_create,		
        cx_func_init,		
        cx_func_removed,		
        cx_func_connect_input,	
        cx_func_connect_output,	
        cx_func_disconnect_input,
        cx_func_disconnect_output,	
        cx_func_last		
    } 







[DllImport("mcw50.dll")]
        public static extern cxobject *cxInputDataGet( int portIndex );

[DllImport("mcw50.dll")]
        public static extern int	cxInputDataGetV( int portIndex, cxobject **data[] );

[DllImport("mcw50.dll")]
        public static extern void	cxOutputDataFlush( int portIndex );
[DllImport("mcw50.dll")]
        public static extern void	cxOutputDataFlushAll( void );
[DllImport("mcw50.dll")]
        public static extern void	cxOutputSendNow( int portIndex );
[DllImport("mcw50.dll")]
        public static extern void 	cxOutputNoSync( void );


[DllImport("mcw50.dll")]
        public static extern void	cxInputDataRelease( int portIndex );
[DllImport("mcw50.dll")]
        public static extern void	cxInputDataReleaseV( int portIndex, int count, int links[] );
[DllImport("mcw50.dll")]
        public static extern void  cxOutputDataRelease( int pId );


[DllImport("mcw50.dll")]
        public static extern long	cxInputDataConnIDGet( int portIndex );
[DllImport("mcw50.dll")]
        public static extern int	cxInputDataConnIDGetV( int portIndex, int *links[] );

[DllImport("mcw50.dll")]
        public static extern int	cxInputDataChanged( int portIndex );
[DllImport("mcw50.dll")]
        public static extern int	cxInputDataChangedV( int portIndex, int *changeVec[] );

[DllImport("mcw50.dll")]
        public static extern int	cxFireDataChanged( void );
[DllImport("mcw50.dll")]
        public static extern int	cxFireDataChangedV( int *changeVec[] );


[DllImport("mcw50.dll")]
        public static extern int	cxInputConnectsGet( int portIndex );
[DllImport("mcw50.dll")]
        public static extern int	cxFireConnectsGet( void );
[DllImport("mcw50.dll")]
        public static extern int	cxOutputConnectsGet( int portIndex );


[DllImport("mcw50.dll")]
        public static extern int cxOutputPortSharedXferGet( int pId );
    */
    }
}
