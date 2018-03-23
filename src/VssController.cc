#include "VssController.h"

VssController::VssController() {
	CoInitialize(nullptr);
	vssapiBase = LoadLibrary((LPCSTR)"vssapi.dll");

	if (vssapiBase) {
		
		CreateVssBackupComponentsInternal_I = (_CreateVssBackupComponentsInternal)GetProcAddress(vssapiBase, "CreateVssBackupComponentsInternal");
		VssFreeSnapshotPropertiesInternal_I = (_VssFreeSnapshotPropertiesInternal)GetProcAddress(vssapiBase, "VssFreeSnapshotPropertiesInternal");
	}

	if (!CreateVssBackupComponentsInternal_I || !VssFreeSnapshotPropertiesInternal_I)
		abort(); // Handle error

	result = CreateVssBackupComponentsInternal_I(&backupComponents);
	if (!SUCCEEDED(result)) {
		cout << "Falha ao instanciar backup components." << endl;
		abort(); // Handle error
	}
}

VssController::~VssController() {
}

void VssController::InicializarBackup() {
	result = backupComponents->InitializeForBackup();
	if (!SUCCEEDED(result)) {
		cout << "Falha ao inicializar componente para backup." << endl;
		MostrarErro(result);
		abort();
	}
}

void VssController::ConfigurarBackup(VSS_BACKUP_TYPE tipo, LONG contexto) {
	result = backupComponents->SetBackupState(TRUE, FALSE, tipo);
	if (!SUCCEEDED(result)) {
		cout << "Falha ao configurar estado do backup." << endl;
		MostrarErro(result);
		abort();
	}

	result = backupComponents->SetContext(contexto);
	if (!SUCCEEDED(result)) {
		cout << "Falha ao configurar contexto do backup." << endl;
		MostrarErro(result);
		abort();
	}
}

VSS_ID VssController::IniciarSnapshotSet() {
	VSS_ID snapshotSetId;
	result = backupComponents->StartSnapshotSet(&snapshotSetId);
	if (!SUCCEEDED(result)) {
		cout << "Falha ao iniciar o snapshot set." << endl;
		MostrarErro(result);
		abort();
	}
	else {
		return snapshotSetId;
	}
}

VSS_ID VssController::AdicionarUnidade(wchar_t * unidade) {
	VSS_ID snapshotId;
	result = backupComponents->AddToSnapshotSet(unidade, GUID_NULL, &snapshotId);
	if (!SUCCEEDED(result)) {
		cout << "Falha ao adicionar a unidade ao snapshot set." << endl;
		MostrarErro(result);
		abort();
	}
	else {
		return snapshotId;
	}
}

void VssController::PepararBackup() {
	IVssAsync *backupPreparation;
	result = backupComponents->PrepareForBackup(&backupPreparation);
	backupPreparation->Wait();
	if (!SUCCEEDED(result)) {
		cout << "Falha ao executar prepara��o do backup." << endl;
		MostrarErro(result);
		abort();
	}
}

void VssController::ExecutarSnapshot() {
	IVssAsync *doSnapshot;
	result = backupComponents->DoSnapshotSet(&doSnapshot);
	doSnapshot->Wait();
	if (!SUCCEEDED(result)) {
		cout << "Falha ao executar o snapshot." << endl;
		MostrarErro(result);
		abort();
	}
}

VSS_SNAPSHOT_PROP VssController::PropriedadesSnapshot(VSS_ID idSnapshot) {
	VSS_SNAPSHOT_PROP propriedadesSnapshot;
	result = backupComponents->GetSnapshotProperties(idSnapshot, &propriedadesSnapshot);
	if (!SUCCEEDED(result)) {
		cout << "Falha ao buscar propriedades do snapshot." << endl;
		MostrarErro(result);
		abort();
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
	if (codigo == E_ACCESSDENIED) {
		std::cout << "The caller does not have sufficient backup privileges or is not an administrator." << std::endl;
	}
	else if (codigo == E_OUTOFMEMORY) {
		std::cout << "The caller is out of memory or other system resources." << std::endl;
	}
	else if (codigo == VSS_E_BAD_STATE) {
		std::cout << "The backup components object is not initialized, this method has been called during a restore operation, or this method has not been called within the correct sequence." << std::endl;
	}
	else if (codigo == VSS_E_INVALID_XML_DOCUMENT) {
		std::cout << "The XML document is not valid. Check the event log for details." << std::endl;
	}
	else if (codigo == VSS_E_UNEXPECTED) {
		std::cout << "Unexpected error. The error code is logged in the error log file." << std::endl;
	}
	else if (codigo == E_INVALIDARG) {
		std::cout << "One of the parameter values is not valid." << std::endl;
	}
	else if (codigo == VSS_E_SNAPSHOT_SET_IN_PROGRESS) {
		std::cout << "The creation of a shadow copy is in progress, and only one shadow copy creation operation can be in progress at one time." << std::endl;
	}
	else if (codigo == VSS_E_MAXIMUM_NUMBER_OF_VOLUMES_REACHED) {
		std::cout << "The maximum number of volumes or remote file shares have been added to the shadow copy set." << std::endl;
	}
	else if (codigo == VSS_E_MAXIMUM_NUMBER_OF_SNAPSHOTS_REACHED) {
		std::cout << "The volume or remote file share has been added to the maximum number of shadow copy sets." << std::endl;
	}
	else if (codigo == VSS_E_WRITER_INFRASTRUCTURE) {
		std::cout << "The writer infrastructure is not operating properly. Check that the Event Service and VSS have been started, and check for errors associated with those services in the error log." << std::endl;
	}
	else if (codigo == VSS_E_NESTED_VOLUME_LIMIT) {
		std::cout << "The specified volume is nested too deeply to participate in the VSS operation." << std::endl;
	}
	else if (codigo == VSS_E_OBJECT_NOT_FOUND) {
		std::cout << "pwszVolumeName does not correspond to an existing volume or remote file share." << std::endl;
	}
	else if (codigo == VSS_E_PROVIDER_NOT_REGISTERED) {
		std::cout << "ProviderId does not correspond to a registered provider." << std::endl;
	}
	else if (codigo == VSS_E_PROVIDER_VETO) {
		std::cout << "Expected provider error. The provider logged the error in the event log." << std::endl;
	}
	else if (codigo == VSS_E_INSUFFICIENT_STORAGE) {
		std::cout << "The system or provider has insufficient storage space. If possible delete any old or unnecessary persistent shadow copies and try again." << std::endl;
	}
	else if (codigo == VSS_E_FLUSH_WRITES_TIMEOUT) {
		std::cout << "The system was unable to flush I/O writes. This can be a transient problem. It is recommended to wait ten minutes and try again, up to three times." << std::endl;
	}
	else if (codigo == VSS_E_HOLD_WRITES_TIMEOUT) {
		std::cout << "The system was unable to hold I/O writes. This can be a transient problem. It is recommended to wait ten minutes and try again, up to three times." << std::endl;
	}
	else if (codigo == VSS_E_REBOOT_REQUIRED) {
		std::cout << "The provider encountered an error that requires the user to restart the computer." << std::endl;
	}
	else if (codigo == VSS_E_TRANSACTION_FREEZE_TIMEOUT) {
		std::cout << "The system was unable to freeze the Distributed Transaction Coordinator (DTC) or the Kernel Transaction Manager (KTM)." << std::endl;
	}
	else if (codigo == VSS_E_TRANSACTION_THAW_TIMEOUT) {
		std::cout << "The system was unable to thaw the Distributed Transaction Coordinator (DTC) or the Kernel Transaction Manager (KTM)." << std::endl;
	}
	else if (codigo == VSS_E_UNEXPECTED_PROVIDER_ERROR) {
		std::cout << "he provider returned an unexpected error code. This can be a transient problem. It is recommended to wait ten minutes and try again, up to three times. " << std::endl;
	}
	else {
		std::cout << "Erro desconhecido." << std::endl;
	}
}