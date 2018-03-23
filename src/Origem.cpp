#include <fstream>
#include "VssController.h"

using namespace std;
ifstream arquivo;
VSS_ID snapshotSetId;
VSS_ID snapshotId;
VSS_SNAPSHOT_PROP propriedadesSnapshot;
VssController *vssController = new VssController();

int main() {

	vssController->InicializarBackup();
	vssController->ConfigurarBackup(VSS_BT_INCREMENTAL, VSS_CTX_BACKUP);
	snapshotSetId = vssController->IniciarSnapshotSet();
	// --------------------
	char volume[] = "C:\\";
	cout << strlen(volume) << endl;
	cout << volume << endl;
	// --------------------
	const size_t cSize = strlen(volume) + 1;
	wstring wc(cSize, L'#');
	mbstowcs(&wc[0], volume, cSize);
	wchar_t* wc2 = const_cast<wchar_t*>(wc.c_str());
	// --------------------
	snapshotId = vssController->AdicionarUnidade(wc2);
	vssController->PepararBackup();
	vssController->ExecutarSnapshot();
	propriedadesSnapshot = vssController->PropriedadesSnapshot(snapshotId);
	wchar_t *snapVol = propriedadesSnapshot.m_pwszSnapshotDeviceObject;
	wstring ws(snapVol);
	string str(ws.begin(), ws.end());
	cout << str << endl;
	// LER ARQUIVO
	string linha;
	arquivo.open(str+"\\teste.txt");
	if (arquivo.is_open()) {
		while (getline(arquivo, linha)) {
			cout << linha << endl;
		}
	}
	arquivo.close();

	vssController->FinalizarBackup(propriedadesSnapshot);

	system("pause");

	return 0;
}