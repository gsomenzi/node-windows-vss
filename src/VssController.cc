#include "VssController.h"

VssController::VssController() {
	CoInitialize(nullptr);
	vssapiBase = LoadLibrary((LPCSTR)"vssapi.dll");

	if (vssapiBase) {
		
		CreateVssBackupComponentsInternal_I = (_CreateVssBackupComponentsInternal)GetProcAddress(vssapiBase, "CreateVssBackupComponentsInternal");
		VssFreeSnapshotPropertiesInternal_I = (_VssFreeSnapshotPropertiesInternal)GetProcAddress(vssapiBase, "VssFreeSnapshotPropertiesInternal");
	}

	if (!CreateVssBackupComponentsInternal_I || !VssFreeSnapshotPropertiesInternal_I)
		MostrarErro((HRESULT)999999);

	result = CreateVssBackupComponentsInternal_I(&backupComponents);
	if (!SUCCEEDED(result)) {
		MostrarErro(result);
	}
}

VssController::~VssController() {
}

string VssController::getCurrentErrMsg() {
	return errMsg;
}

void VssController::InicializarBackup() {
	result = backupComponents->InitializeForBackup();
	if (!SUCCEEDED(result)) {
		MostrarErro(result);
	}
}

void VssController::ConfigurarBackup(VSS_BACKUP_TYPE tipo, LONG contexto) {
	result = backupComponents->SetBackupState(TRUE, FALSE, tipo);
	if (!SUCCEEDED(result)) {
		MostrarErro(result);
	}

	result = backupComponents->SetContext(contexto);
	if (!SUCCEEDED(result)) {
		MostrarErro(result);
	}
}

VSS_ID VssController::IniciarSnapshotSet() {
	VSS_ID snapshotSetId;
	result = backupComponents->StartSnapshotSet(&snapshotSetId);
	if (!SUCCEEDED(result)) {
		MostrarErro(result);
	}
	else {
		return snapshotSetId;
	}
}

VSS_ID VssController::AdicionarUnidade(wchar_t * unidade) {
	VSS_ID snapshotId;
	result = backupComponents->AddToSnapshotSet(unidade, GUID_NULL, &snapshotId);
	if (!SUCCEEDED(result)) {
		MostrarErro(result);
	}
	return snapshotId;
}

void VssController::PepararBackup() {
	IVssAsync *backupPreparation;
	result = backupComponents->PrepareForBackup(&backupPreparation);
	backupPreparation->Wait();
	if (!SUCCEEDED(result)) {
		MostrarErro(result);
	}
}

void VssController::ExecutarSnapshot() {
	IVssAsync *doSnapshot;
	result = backupComponents->DoSnapshotSet(&doSnapshot);
	doSnapshot->Wait();
	if (!SUCCEEDED(result)) {
		MostrarErro(result);
	}
}

VSS_SNAPSHOT_PROP VssController::PropriedadesSnapshot(VSS_ID idSnapshot) {
	VSS_SNAPSHOT_PROP propriedadesSnapshot;
	result = backupComponents->GetSnapshotProperties(idSnapshot, &propriedadesSnapshot);
	if (!SUCCEEDED(result)) {
		MostrarErro(result);
	}
	else {
		return propriedadesSnapshot;
	}
}

void VssController::FinalizarBackup(VSS_SNAPSHOT_PROP propriedadesSnapshot) {
	VssFreeSnapshotPropertiesInternal_I(&propriedadesSnapshot);
	backupComponents->Release();
}

void VssController::MostrarErro(HRESULT codigo) {
	if (codigo == 999999) {
		errMsg.assign("Fail trying to instantiate vss functions.");
	} else if (codigo == E_ACCESSDENIED) {
		errMsg.assign("The caller does not have sufficient backup privileges or is not an administrator.");
	}
	else if (codigo == E_OUTOFMEMORY) {
		errMsg.assign("The caller is out of memory or other system resources.");
	}
	else if (codigo == VSS_E_BAD_STATE) {
		errMsg.assign("The backup components object is not initialized, this method has been called during a restore operation, or this method has not been called within the correct sequence.");
	}
	else if (codigo == VSS_E_INVALID_XML_DOCUMENT) {
		errMsg.assign("The XML document is not valid. Check the event log for details.");
	}
	else if (codigo == VSS_E_UNEXPECTED) {
		errMsg.assign("Unexpected error. The error code is logged in the error log file.");
	}
	else if (codigo == E_INVALIDARG) {
		errMsg.assign("One of the parameter values is not valid.");
	}
	else if (codigo == VSS_E_SNAPSHOT_SET_IN_PROGRESS) {
		errMsg.assign("The creation of a shadow copy is in progress, and only one shadow copy creation operation can be in progress at one time.");
	}
	else if (codigo == VSS_E_MAXIMUM_NUMBER_OF_VOLUMES_REACHED) {
		errMsg.assign("The maximum number of volumes or remote file shares have been added to the shadow copy set.");
	}
	else if (codigo == VSS_E_MAXIMUM_NUMBER_OF_SNAPSHOTS_REACHED) {
		errMsg.assign("The volume or remote file share has been added to the maximum number of shadow copy sets.");
	}
	else if (codigo == VSS_E_WRITER_INFRASTRUCTURE) {
		errMsg.assign("The writer infrastructure is not operating properly. Check that the Event Service and VSS have been started, and check for errors associated with those services in the error log.");
	}
	else if (codigo == VSS_E_NESTED_VOLUME_LIMIT) {
		errMsg.assign("The specified volume is nested too deeply to participate in the VSS operation.");
	}
	else if (codigo == VSS_E_OBJECT_NOT_FOUND) {
		errMsg.assign("pwszVolumeName does not correspond to an existing volume or remote file share.");
	}
	else if (codigo == VSS_E_PROVIDER_NOT_REGISTERED) {
		errMsg.assign("ProviderId does not correspond to a registered provider.");
	}
	else if (codigo == VSS_E_PROVIDER_VETO) {
		errMsg.assign("Expected provider error. The provider logged the error in the event log.");
	}
	else if (codigo == VSS_E_INSUFFICIENT_STORAGE) {
		errMsg.assign("The system or provider has insufficient storage space. If possible delete any old or unnecessary persistent shadow copies and try again.");
	}
	else if (codigo == VSS_E_FLUSH_WRITES_TIMEOUT) {
		errMsg.assign("The system was unable to flush I/O writes. This can be a transient problem. It is recommended to wait ten minutes and try again, up to three times.");
	}
	else if (codigo == VSS_E_HOLD_WRITES_TIMEOUT) {
		errMsg.assign("The system was unable to hold I/O writes. This can be a transient problem. It is recommended to wait ten minutes and try again, up to three times.");
	}
	else if (codigo == VSS_E_REBOOT_REQUIRED) {
		errMsg.assign("The provider encountered an error that requires the user to restart the computer.");
	}
	else if (codigo == VSS_E_TRANSACTION_FREEZE_TIMEOUT) {
		errMsg.assign("The system was unable to freeze the Distributed Transaction Coordinator (DTC) or the Kernel Transaction Manager (KTM).");
	}
	else if (codigo == VSS_E_TRANSACTION_THAW_TIMEOUT) {
		errMsg.assign("The system was unable to thaw the Distributed Transaction Coordinator (DTC) or the Kernel Transaction Manager (KTM).");
	}
	else if (codigo == VSS_E_UNEXPECTED_PROVIDER_ERROR) {
		errMsg.assign("he provider returned an unexpected error code. This can be a transient problem. It is recommended to wait ten minutes and try again, up to three times. ");
	}
	else {
		errMsg.assign("Unknown error.");
	}
}