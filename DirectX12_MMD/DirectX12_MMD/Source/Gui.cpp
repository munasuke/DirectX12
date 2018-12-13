#include "Gui.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"

#include <cassert>

Gui::Gui() : heap(nullptr), rs(nullptr), flag(false) {
	ImGui::CreateContext();
}

void Gui::Initialize(ID3D12Device * dev, HWND hwnd) {
	//imgui用のヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDsc = {};
	heapDsc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDsc.NodeMask		= 0;
	heapDsc.NumDescriptors	= 1;
	heapDsc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	auto result = dev->CreateDescriptorHeap(&heapDsc, IID_PPV_ARGS(&heap));

	//サンプラ
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter				= D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU			= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV			= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW			= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MaxLOD				= D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD				= 0.0f;
	samplerDesc.MipLODBias			= 0.0f;
	samplerDesc.BorderColor			= D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.ShaderRegister		= 0;
	samplerDesc.ShaderVisibility	= D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc.RegisterSpace		= 0;
	samplerDesc.MaxAnisotropy		= 0;
	samplerDesc.ComparisonFunc		= D3D12_COMPARISON_FUNC_NEVER;

	//デスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE dr = {};
	dr.BaseShaderRegister					= 0;
	dr.NumDescriptors						= 1;
	dr.OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	dr.RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE::D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	dr.RegisterSpace						= 0;

	//ルートパラメータ
	D3D12_ROOT_PARAMETER rp = {};
	rp.ParameterType						= D3D12_ROOT_PARAMETER_TYPE::D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp.ShaderVisibility						= D3D12_SHADER_VISIBILITY::D3D12_SHADER_VISIBILITY_ALL;
	rp.DescriptorTable.NumDescriptorRanges	= 1;
	rp.DescriptorTable.pDescriptorRanges	= &dr;

	//ルートシグネチャを生成
	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters		= 1;
	rsDesc.NumStaticSamplers	= 1;
	rsDesc.pParameters			= &rp;
	rsDesc.pStaticSamplers		= &samplerDesc;
	rsDesc.Flags				= D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signature = nullptr;
	ID3DBlob* error		= nullptr;

	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION::D3D_ROOT_SIGNATURE_VERSION_1,
		&signature,
		&error
	);

	result = dev->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rs)
	);

	//ImGui初期化
	auto guiresult = ImGui_ImplWin32_Init(hwnd);
	assert(guiresult);

	guiresult = ImGui_ImplDX12_Init(
		dev,
		2,
		DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		heap->GetCPUDescriptorHandleForHeapStart(),
		heap->GetGPUDescriptorHandleForHeapStart()
	);
	assert(guiresult);
}

void Gui::BeforeDrawing() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Gui::Draw(ID3D12GraphicsCommandList * list) {
	//処理の開始
	ImGui::Begin("test");

	//UIウィンドウのサイズを指定。ImGuiCond_Always（常に）
	ImGui::SetNextWindowSize({ 400.0f, 500.0f }, ImGuiCond_Always);

	//UIウィンドウの座標を指定。ImGuiCond_Once（実行時に一度のみ）
	ImGui::SetWindowPos({ 50, 60 }, ImGuiCond_Once);

	//点とテキスト
	ImGui::BulletText("buhi");

	//点のみ
	ImGui::Bullet();
	//テキストを着色
	ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "Green");

	//文字入力できるようにする
	static char str[128] = "Fuck You !!!";
	ImGui::InputText("InputText", str, IM_ARRAYSIZE(str));

	//矢印ボタン。ImGuiDir_Down（下）
	if(ImGui::ArrowButton("type", ImGuiDir_Down)) {
		flag = !flag;
	}
	//改行しない
	ImGui::SameLine();
	ImGui::Text("Click Here");

	if (flag) {
		ImGui::Bullet();
		ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "Hello !!!");
	}

	//処理の終了
	ImGui::End();

	//処理の描画
	ImGui::Render();

	list->SetGraphicsRootSignature(rs);
	list->SetDescriptorHeaps(1, &heap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), list);
}


Gui::~Gui() {
	//処理の解放
	ImGui_ImplDX12_Shutdown();
}
