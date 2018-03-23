#include <nan.h>
#include "src/VssController.h"

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
	String::Utf8Value s(info[0]);
	// PARSE E CONVERTE UNIDADE DE DISCO
	std::string strinput(*s);
	char * writable = new char[strinput.size() + 1];
	std::copy(strinput.begin(), strinput.end(), writable);
	writable[strinput.size()] = '\0';
	const size_t cSize = strlen(writable) + 1;
	wstring wc(cSize, L'#');
	mbstowcs(&wc[0], writable, cSize);
	wchar_t* wc2 = const_cast<wchar_t*>(wc.c_str());
	// VSS
	VssController *vssController = new VssController();
	vssController->InicializarBackup();
	vssController->ConfigurarBackup(VSS_BT_INCREMENTAL, VSS_CTX_BACKUP);
	snapshotSetId = vssController->IniciarSnapshotSet();
	snapshotId = vssController->AdicionarUnidade(wc2);
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

void Init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
  Nan::SetMethod(module, "exports", RunCallback);
}

NODE_MODULE(addon, Init)