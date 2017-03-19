/*++

Module Name:

    miniFilterBlockGame.c

Abstract:

    This is the main module of the miniFilter_BlockGame miniFilter driver.

Environment:

    Kernel mode

--*/

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include "CallBacks.h"

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")


PFLT_FILTER gFilterHandle;
ULONG_PTR OperationStatusCtx = 1;

#define PTDBG_TRACE_ROUTINES            0x00000001
#define PTDBG_TRACE_OPERATION_STATUS    0x00000002

ULONG gTraceFlags = 0;


#define PT_DBG_PRINT( _dbgLevel, _string )          \
    (FlagOn(gTraceFlags,(_dbgLevel)) ?              \
        DbgPrint _string :                          \
        ((int)0))


//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, miniFilterBlockGameUnload)
#pragma alloc_text(PAGE, miniFilterBlockGameInstanceQueryTeardown)
#pragma alloc_text(PAGE, miniFilterBlockGameInstanceSetup)
#pragma alloc_text(PAGE, miniFilterBlockGameInstanceTeardownStart)
#pragma alloc_text(PAGE, miniFilterBlockGameInstanceTeardownComplete)
#endif

const UNICODE_STRING FilteringProcessList[] =
{
	RTL_CONSTANT_STRING(L"League of Legends.exe"),
	RTL_CONSTANT_STRING(L"MapleStory.exe"),
	{ 0,0,NULL },
};

//
//  operation registration
//

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {

#if 0 // TODO - List all of the requests to filter.
    { IRP_MJ_CREATE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_CREATE_NAMED_PIPE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_CLOSE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_READ,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_WRITE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_QUERY_INFORMATION,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_SET_INFORMATION,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_QUERY_EA,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_SET_EA,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_FLUSH_BUFFERS,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_QUERY_VOLUME_INFORMATION,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_SET_VOLUME_INFORMATION,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_DIRECTORY_CONTROL,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_FILE_SYSTEM_CONTROL,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_DEVICE_CONTROL,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_INTERNAL_DEVICE_CONTROL,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_SHUTDOWN,
      0,
      miniFilterBlockGamePreOperationNoPostOperation,
      NULL },                               //post operations not supported

    { IRP_MJ_LOCK_CONTROL,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_CLEANUP,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_CREATE_MAILSLOT,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_QUERY_SECURITY,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_SET_SECURITY,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_QUERY_QUOTA,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_SET_QUOTA,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_PNP,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_RELEASE_FOR_SECTION_SYNCHRONIZATION,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_ACQUIRE_FOR_MOD_WRITE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_RELEASE_FOR_MOD_WRITE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_ACQUIRE_FOR_CC_FLUSH,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_RELEASE_FOR_CC_FLUSH,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_FAST_IO_CHECK_IF_POSSIBLE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_NETWORK_QUERY_OPEN,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_MDL_READ,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_MDL_READ_COMPLETE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_PREPARE_MDL_WRITE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_MDL_WRITE_COMPLETE,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_VOLUME_MOUNT,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

    { IRP_MJ_VOLUME_DISMOUNT,
      0,
      miniFilterBlockGamePreOperation,
      miniFilterBlockGamePostOperation },

#endif // TODO

    { IRP_MJ_OPERATION_END }
};

//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof( FLT_REGISTRATION ),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    NULL,                               //  Context
    Callbacks,                          //  Operation callbacks

    miniFilterBlockGameUnload,                           //  MiniFilterUnload

    miniFilterBlockGameInstanceSetup,                    //  InstanceSetup
    miniFilterBlockGameInstanceQueryTeardown,            //  InstanceQueryTeardown
    miniFilterBlockGameInstanceTeardownStart,            //  InstanceTeardownStart
    miniFilterBlockGameInstanceTeardownComplete,         //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent

};



NTSTATUS
miniFilterBlockGameInstanceSetup (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
/*++

Routine Description:

    This routine is called whenever a new instance is created on a volume. This
    gives us a chance to decide if we need to attach to this volume or not.

    If this routine is not defined in the registration structure, automatic
    instances are always created.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Flags describing the reason for this attach request.

Return Value:

    STATUS_SUCCESS - attach
    STATUS_FLT_DO_NOT_ATTACH - do not attach

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeDeviceType );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!miniFilterBlockGameInstanceSetup: Entered\n") );

    return STATUS_SUCCESS;
}


NTSTATUS
miniFilterBlockGameInstanceQueryTeardown (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This is called when an instance is being manually deleted by a
    call to FltDetachVolume or FilterDetach thereby giving us a
    chance to fail that detach request.

    If this routine is not defined in the registration structure, explicit
    detach requests via FltDetachVolume or FilterDetach will always be
    failed.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Indicating where this detach request came from.

Return Value:

    Returns the status of this operation.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!miniFilterBlockGameInstanceQueryTeardown: Entered\n") );

    return STATUS_SUCCESS;
}


VOID
miniFilterBlockGameInstanceTeardownStart (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This routine is called at the start of instance teardown.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Reason why this instance is being deleted.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!miniFilterBlockGameInstanceTeardownStart: Entered\n") );
}


VOID
miniFilterBlockGameInstanceTeardownComplete (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This routine is called at the end of instance teardown.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Reason why this instance is being deleted.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!miniFilterBlockGameInstanceTeardownComplete: Entered\n") );
}


/*************************************************************************
    MiniFilter initialization and unload routines.
*************************************************************************/

NTSTATUS
DriverEntry (
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This is the initialization routine for this miniFilter driver.  This
    registers with FltMgr and initializes all global data structures.

Arguments:

    DriverObject - Pointer to driver object created by the system to
        represent this driver.

    RegistryPath - Unicode string identifying where the parameters for this
        driver are located in the registry.

Return Value:

    Routine can return non success error codes.

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER( RegistryPath );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!DriverEntry: Entered\n") );

    //
    //  Register with FltMgr to tell it our callback routines
    //

    status = FltRegisterFilter( DriverObject,
                                &FilterRegistration,
                                &gFilterHandle );

    FLT_ASSERT( NT_SUCCESS( status ) );

    if (NT_SUCCESS( status )) {

        //
        //  Start filtering i/o
        //

        status = FltStartFiltering( gFilterHandle );

        if (!NT_SUCCESS( status )) {

            FltUnregisterFilter( gFilterHandle );
        }
    }

	PsSetCreateProcessNotifyRoutineEx(ProcessNotifyCallBackRoutine, FALSE);
	//PsSetLoadImageNotifyRoutine(LoadImageCallBackRoutine);
	


    return status;
}

NTSTATUS
miniFilterBlockGameUnload (
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
    )
/*++

Routine Description:

    This is the unload routine for this miniFilter driver. This is called
    when the minifilter is about to be unloaded. We can fail this unload
    request if this is not a mandatory unload indicated by the Flags
    parameter.

Arguments:

    Flags - Indicating if this is a mandatory unload.

Return Value:

    Returns STATUS_SUCCESS.

--*/
{
    UNREFERENCED_PARAMETER( Flags );

    PAGED_CODE();

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!miniFilterBlockGameUnload: Entered\n") );

	PsSetCreateProcessNotifyRoutineEx(ProcessNotifyCallBackRoutine, TRUE);
	//PsRemoveLoadImageNotifyRoutine(LoadImageCallBackRoutine);

    FltUnregisterFilter( gFilterHandle );

    return STATUS_SUCCESS;
}


/*************************************************************************
    MiniFilter callback routines.
*************************************************************************/
FLT_PREOP_CALLBACK_STATUS
miniFilterBlockGamePreOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is a pre-operation dispatch routine for this miniFilter.

    This is non-pageable because it could be called on the paging path

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - The context for the completion routine for this
        operation.

Return Value:

    The return value is the status of the operation.

--*/
{
    NTSTATUS status;

    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!miniFilterBlockGamePreOperation: Entered\n") );

    //
    //  See if this is an operation we would like the operation status
    //  for.  If so request it.
    //
    //  NOTE: most filters do NOT need to do this.  You only need to make
    //        this call if, for example, you need to know if the oplock was
    //        actually granted.
    //

    if (miniFilterBlockGameDoRequestOperationStatus( Data )) {

        status = FltRequestOperationStatusCallback( Data,
                                                    miniFilterBlockGameOperationStatusCallback,
                                                    (PVOID)(++OperationStatusCtx) );
        if (!NT_SUCCESS(status)) {

            PT_DBG_PRINT( PTDBG_TRACE_OPERATION_STATUS,
                          ("miniFilterBlockGame!miniFilterBlockGamePreOperation: FltRequestOperationStatusCallback Failed, status=%08x\n",
                           status) );
        }
    }

    // This template code does not do anything with the callbackData, but
    // rather returns FLT_PREOP_SUCCESS_WITH_CALLBACK.
    // This passes the request down to the next miniFilter in the chain.

    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}



VOID
miniFilterBlockGameOperationStatusCallback (
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
    _In_ NTSTATUS OperationStatus,
    _In_ PVOID RequesterContext
    )
/*++

Routine Description:

    This routine is called when the given operation returns from the call
    to IoCallDriver.  This is useful for operations where STATUS_PENDING
    means the operation was successfully queued.  This is useful for OpLocks
    and directory change notification operations.

    This callback is called in the context of the originating thread and will
    never be called at DPC level.  The file object has been correctly
    referenced so that you can access it.  It will be automatically
    dereferenced upon return.

    This is non-pageable because it could be called on the paging path

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    RequesterContext - The context for the completion routine for this
        operation.

    OperationStatus -

Return Value:

    The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER( FltObjects );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!miniFilterBlockGameOperationStatusCallback: Entered\n") );

    PT_DBG_PRINT( PTDBG_TRACE_OPERATION_STATUS,
                  ("miniFilterBlockGame!miniFilterBlockGameOperationStatusCallback: Status=%08x ctx=%p IrpMj=%02x.%02x \"%s\"\n",
                   OperationStatus,
                   RequesterContext,
                   ParameterSnapshot->MajorFunction,
                   ParameterSnapshot->MinorFunction,
                   FltGetIrpName(ParameterSnapshot->MajorFunction)) );
}


FLT_POSTOP_CALLBACK_STATUS
miniFilterBlockGamePostOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
    )
/*++

Routine Description:

    This routine is the post-operation completion routine for this
    miniFilter.

    This is non-pageable because it may be called at DPC level.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - The completion context set in the pre-operation routine.

    Flags - Denotes whether the completion is successful or is being drained.

Return Value:

    The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );
    UNREFERENCED_PARAMETER( Flags );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!miniFilterBlockGamePostOperation: Entered\n") );

    return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
miniFilterBlockGamePreOperationNoPostOperation (
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is a pre-operation dispatch routine for this miniFilter.

    This is non-pageable because it could be called on the paging path

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - The context for the completion routine for this
        operation.

Return Value:

    The return value is the status of the operation.

--*/
{
    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );

    PT_DBG_PRINT( PTDBG_TRACE_ROUTINES,
                  ("miniFilterBlockGame!miniFilterBlockGamePreOperationNoPostOperation: Entered\n") );

    // This template code does not do anything with the callbackData, but
    // rather returns FLT_PREOP_SUCCESS_NO_CALLBACK.
    // This passes the request down to the next miniFilter in the chain.

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

BOOLEAN
miniFilterBlockGameDoRequestOperationStatus(
    _In_ PFLT_CALLBACK_DATA Data
    )
/*++

Routine Description:

    This identifies those operations we want the operation status for.  These
    are typically operations that return STATUS_PENDING as a normal completion
    status.

Arguments:

Return Value:

    TRUE - If we want the operation status
    FALSE - If we don't

--*/
{
    PFLT_IO_PARAMETER_BLOCK iopb = Data->Iopb;

    //
    //  return boolean state based on which operations we are interested in
    //

    return (BOOLEAN)

            //
            //  Check for oplock operations
            //

             (((iopb->MajorFunction == IRP_MJ_FILE_SYSTEM_CONTROL) &&
               ((iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_FILTER_OPLOCK)  ||
                (iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_BATCH_OPLOCK)   ||
                (iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_1) ||
                (iopb->Parameters.FileSystemControl.Common.FsControlCode == FSCTL_REQUEST_OPLOCK_LEVEL_2)))

              ||

              //
              //    Check for directy change notification
              //

              ((iopb->MajorFunction == IRP_MJ_DIRECTORY_CONTROL) &&
               (iopb->MinorFunction == IRP_MN_NOTIFY_CHANGE_DIRECTORY))
             );
}


VOID ProcessNotifyCallBackRoutine(
	_Inout_ PEPROCESS Process,
	_In_ HANDLE ProcessId,
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
	UNREFERENCED_PARAMETER(Process);
	UNREFERENCED_PARAMETER(ProcessId);
	UNICODE_STRING FileName;
	if (CreateInfo != NULL)	
	{
		/*DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "ProcessName(obj->name) : %ws", CreateInfo->FileObject->FileName.Buffer);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "ProcessName(CmdLine) : %ws", CreateInfo->CommandLine->Buffer);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "ProcessName(imgFileName) : %ws", CreateInfo->ImageFileName->Buffer);*/
		FltParseFileName(&CreateInfo->FileObject->FileName, NULL, NULL, &FileName);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "ProcessName : %ws", FileName.Buffer);
		if (ProcessFiltering(&FileName))
		{
			CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, 0, "Denied ProcessName : %ws", FileName.Buffer);
		}
	}
}

//VOID LoadImageCallBackRoutine(
//	_In_opt_ PUNICODE_STRING FullImageName,
//	_In_ HANDLE ProcessId,               
//	_In_ PIMAGE_INFO ImageInfo
//)
//{
//
//}

BOOLEAN ProcessFiltering(
	_In_ PUNICODE_STRING pCompareString
) {
	const UNICODE_STRING* str;

	if (pCompareString->Length == 0)
	{
		return FALSE;
	}

	str = FilteringProcessList;

	//넘어온확장자가ext의리스트와동일하다면TRUE를반환해준다.
	//해당하는확장자만필터링하기위함이다.
	while (str->Buffer != NULL)
	{
		if (RtlCompareUnicodeString(pCompareString, str, FALSE) == 0)
		{
			return TRUE;
		}
		str++;
	}
	return FALSE;
}