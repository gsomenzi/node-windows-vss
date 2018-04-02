#include <nan.h>
#include <string>
#include "VssController.h"

using v8::Function;
using v8::Local;
using v8::Number;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;
using v8::String;

VSS_ID snapshotSetId;
VSS_ID snapshotId;
VSS_SNAPSHOT_PROP propriedadesSnapshot;

void RunCallback(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	// DEFINE VARIAVEIS
	v8::Local<v8::Function> cb = info[1].As<v8::Function>();
	String::Utf8Value utf8val(info[0]);
	// PARSE E CONVERTE UNIDADE DE DISCO
	std::string strvolume(*utf8val);
	if (strvolume.back() != '\\') {
		strvolume = strvolume + '\\';
	}
	char * charvolume = new char[strvolume.size() + 1];
	std::copy(strvolume.begin(), strvolume.end(), charvolume);
	charvolume[strvolume.size()] = '\0';
	const size_t charvolumesize = strlen(charvolume) + 1;
	wstring wstrvolume(charvolumesize, L'#');
	mbstowcs(&wstrvolume[0], charvolume, charvolumesize);
	wchar_t* wcharvolume = const_cast<wchar_t*>(wstrvolume.c_str());
	// VSS
	VssController *vssController = new VssController();
	// IF CANNOT INITIALIZE BACKUP
	if (!vssController->getCurrentErrMsg().empty()) {
		v8::Local<v8::Value> argv[2] = { Nan::New(vssController->getCurrentErrMsg()).ToLocalChecked(), Nan::Null() };
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, 2, argv);
		exit(0);
	}
	vssController->InicializarBackup();
	// IF CANNOT INITIALIZE BACKUP
	if (!vssController->getCurrentErrMsg().empty()) {
		v8::Local<v8::Value> argv[2] = { Nan::New(vssController->getCurrentErrMsg()).ToLocalChecked(), Nan::Null() };
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, 2, argv);
		exit(0);
	}
	vssController->ConfigurarBackup(VSS_BT_INCREMENTAL, VSS_CTX_BACKUP);
	// IF CANNOT INITIALIZE BACKUP
	if (!vssController->getCurrentErrMsg().empty()) {
		v8::Local<v8::Value> argv[2] = { Nan::New(vssController->getCurrentErrMsg()).ToLocalChecked(), Nan::Null() };
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, 2, argv);
		exit(0);
	}
	snapshotSetId = vssController->IniciarSnapshotSet();
	snapshotId = vssController->AdicionarUnidade(wcharvolume);
	vssController->PepararBackup();
	vssController->ExecutarSnapshot();
	propriedadesSnapshot = vssController->PropriedadesSnapshot(snapshotId);
	wchar_t *snapVol = propriedadesSnapshot.m_pwszSnapshotDeviceObject;
	wstring ws(snapVol);
	string str(ws.begin(), ws.end());
	vssController->FinalizarBackup(propriedadesSnapshot);
	// CALLBACK
	v8::Local<v8::Value> argv[2] = { Nan::Null(), Nan::New(str).ToLocalChecked() };
	Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, 2, argv);
}

//void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
//  Nan::SetMethod(module, "exports", RunCallback);
//}

void Init(v8::Local<v8::Object> exports) {
	exports->Set(Nan::New("snapshot").ToLocalChecked(),
		Nan::New<v8::FunctionTemplate>(RunCallback)->GetFunction());
}

NODE_MODULE(addon, Init)