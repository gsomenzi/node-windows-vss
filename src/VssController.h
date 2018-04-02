#include <vss.h>
#include <vswriter.h>
#include <vsbackup.h>
#include <iostream>
#include <string>

using namespace std;

typedef HRESULT(STDAPICALLTYPE *_CreateVssBackupComponentsInternal)(
__out IVssBackupComponents **ppBackup
);

typedef void (APIENTRY *_VssFreeSnapshotPropertiesInternal)(
__in VSS_SNAPSHOT_PROP *pProp
);

static _CreateVssBackupComponentsInternal CreateVssBackupComponentsInternal_I;
static _VssFreeSnapshotPropertiesInternal VssFreeSnapshotPropertiesInternal_I;

class VssController {
private:
	IVssBackupComponents * backupComponents;
	HMODULE vssapiBase;
	HRESULT result;
	string errMsg;
public:
	VssController();
	~VssController();
	void MostrarErro(HRESULT codigo);
	void InicializarBackup();
	void ConfigurarBackup(VSS_BACKUP_TYPE tipo, LONG contexto);
	VSS_ID IniciarSnapshotSet();
	VSS_ID AdicionarUnidade(wchar_t * unidade);
	void PepararBackup();
	void ExecutarSnapshot();
	VSS_SNAPSHOT_PROP PropriedadesSnapshot(VSS_ID idSnapshot);
	void FinalizarBackup(VSS_SNAPSHOT_PROP propriedadesSnapshot);
	string getCurrentErrMsg();
};

