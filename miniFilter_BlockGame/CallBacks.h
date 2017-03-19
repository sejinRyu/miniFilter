#pragma once

#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>


/*************************************************************************
Prototypes
*************************************************************************/

EXTERN_C_START

DRIVER_INITIALIZE DriverEntry;
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
);

NTSTATUS
miniFilterBlockGameInstanceSetup(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	_In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
);

VOID
miniFilterBlockGameInstanceTeardownStart(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

VOID
miniFilterBlockGameInstanceTeardownComplete(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

NTSTATUS
miniFilterBlockGameUnload(
	_In_ FLT_FILTER_UNLOAD_FLAGS Flags
);

NTSTATUS
miniFilterBlockGameInstanceQueryTeardown(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

FLT_PREOP_CALLBACK_STATUS
miniFilterBlockGamePreOperation(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

VOID
miniFilterBlockGameOperationStatusCallback(
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_ PFLT_IO_PARAMETER_BLOCK ParameterSnapshot,
	_In_ NTSTATUS OperationStatus,
	_In_ PVOID RequesterContext
);

FLT_POSTOP_CALLBACK_STATUS
miniFilterBlockGamePostOperation(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
);

FLT_PREOP_CALLBACK_STATUS
miniFilterBlockGamePreOperationNoPostOperation(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

BOOLEAN
miniFilterBlockGameDoRequestOperationStatus(
	_In_ PFLT_CALLBACK_DATA Data
);

VOID ProcessNotifyCallBackRoutine(
	_Inout_ PEPROCESS Process,
	_In_ HANDLE ProcessId,
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
);

//VOID LoadImageCallBackRoutine(
//	_In_opt_ PUNICODE_STRING FullImageName,
//	_In_ HANDLE ProcessId,
//	_In_ PIMAGE_INFO ImageInfo
//);

BOOLEAN ProcessFiltering(
	_In_ PUNICODE_STRING pCompareString
);

EXTERN_C_END
