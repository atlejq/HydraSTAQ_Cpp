#pragma once
#include <stdlib.h>     /* srand, rand */
#include <stdexcept>

extern std::string path;
extern std::string parameterDir;
extern std::string lightDir;
extern std::string ext;
extern float detectionThreshold;
extern float discardPercentage;
extern int maxStars;
extern int topMatches;
extern std::string filter;
extern std::string align;

namespace CppCLRWinFormsProject {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	public:int ReadImages();

	public:int ComputeOffsets();

	public:int Stack();

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ button1;
	private: System::Windows::Forms::Button^ button2;
	private: System::Windows::Forms::Button^ button3;
	private: System::Windows::Forms::RadioButton^ radioButton1;
	private: System::Windows::Forms::RadioButton^ radioButton2;
	private: System::Windows::Forms::RadioButton^ radioButton3;
	private: System::Windows::Forms::RadioButton^ radioButton4;
	private: System::Windows::Forms::RadioButton^ radioButton5;
	private: System::Windows::Forms::RadioButton^ radioButton6;
	private: System::Windows::Forms::RadioButton^ radioButton7;
	private: System::Windows::Forms::RadioButton^ radioButton8;
	private: System::Windows::Forms::TextBox^ textBox10;

	private: System::Windows::Forms::TrackBar^ trackBar1;
	private: System::Windows::Forms::TextBox^ textBox1;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::Panel^ panel2;

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButton8 = (gcnew System::Windows::Forms::RadioButton());
			this->textBox10 = (gcnew System::Windows::Forms::TextBox());
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->BeginInit();
			this->panel1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(10, 20);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(74, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"Read";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(10, 60);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(74, 23);
			this->button2->TabIndex = 7;
			this->button2->Text = L"Offsets";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(10, 100);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(74, 23);
			this->button3->TabIndex = 8;
			this->button3->Text = L"Stack";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// radioButton1
			// 
			this->radioButton1->AutoSize = true;
			this->radioButton1->Checked = true;
			this->radioButton1->Location = System::Drawing::Point(14, 9);
			this->radioButton1->Name = L"radioButton1";
			this->radioButton1->Size = System::Drawing::Size(75, 20);
			this->radioButton1->TabIndex = 9;
			this->radioButton1->TabStop = true;
			this->radioButton1->Text = L"Stack R";
			this->radioButton1->UseVisualStyleBackColor = true;
			this->radioButton1->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton1_CheckedChanged);
			// 
			// radioButton2
			// 
			this->radioButton2->AutoSize = true;
			this->radioButton2->Location = System::Drawing::Point(14, 49);
			this->radioButton2->Name = L"radioButton2";
			this->radioButton2->Size = System::Drawing::Size(75, 20);
			this->radioButton2->TabIndex = 10;
			this->radioButton2->TabStop = true;
			this->radioButton2->Text = L"Stack G";
			this->radioButton2->UseVisualStyleBackColor = true;
			this->radioButton2->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton2_CheckedChanged);
			// 
			// radioButton3
			// 
			this->radioButton3->AutoSize = true;
			this->radioButton3->Location = System::Drawing::Point(14, 89);
			this->radioButton3->Name = L"radioButton3";
			this->radioButton3->Size = System::Drawing::Size(74, 20);
			this->radioButton3->TabIndex = 11;
			this->radioButton3->TabStop = true;
			this->radioButton3->Text = L"Stack B";
			this->radioButton3->UseVisualStyleBackColor = true;
			this->radioButton3->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton3_CheckedChanged);
			// 
			// radioButton4
			// 
			this->radioButton4->AutoSize = true;
			this->radioButton4->Location = System::Drawing::Point(14, 129);
			this->radioButton4->Name = L"radioButton4";
			this->radioButton4->Size = System::Drawing::Size(72, 20);
			this->radioButton4->TabIndex = 12;
			this->radioButton4->TabStop = true;
			this->radioButton4->Text = L"Stack L";
			this->radioButton4->UseVisualStyleBackColor = true;
			this->radioButton4->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton4_CheckedChanged);
			// 
			// radioButton5
			// 
			this->radioButton5->AutoSize = true;
			this->radioButton5->Checked = true;
			this->radioButton5->Location = System::Drawing::Point(19, 9);
			this->radioButton5->Name = L"radioButton5";
			this->radioButton5->Size = System::Drawing::Size(89, 20);
			this->radioButton5->TabIndex = 13;
			this->radioButton5->TabStop = true;
			this->radioButton5->Text = L"Align by R";
			this->radioButton5->UseVisualStyleBackColor = true;
			this->radioButton5->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton5_CheckedChanged);
			// 
			// radioButton6
			// 
			this->radioButton6->AutoSize = true;
			this->radioButton6->Location = System::Drawing::Point(19, 49);
			this->radioButton6->Name = L"radioButton6";
			this->radioButton6->Size = System::Drawing::Size(89, 20);
			this->radioButton6->TabIndex = 14;
			this->radioButton6->TabStop = true;
			this->radioButton6->Text = L"Align by G";
			this->radioButton6->UseVisualStyleBackColor = true;
			this->radioButton6->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton6_CheckedChanged);
			// 
			// radioButton7
			// 
			this->radioButton7->AutoSize = true;
			this->radioButton7->Location = System::Drawing::Point(19, 89);
			this->radioButton7->Name = L"radioButton7";
			this->radioButton7->Size = System::Drawing::Size(88, 20);
			this->radioButton7->TabIndex = 15;
			this->radioButton7->TabStop = true;
			this->radioButton7->Text = L"Align by B";
			this->radioButton7->UseVisualStyleBackColor = true;
			this->radioButton7->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton7_CheckedChanged);
			// 
			// radioButton8
			// 
			this->radioButton8->AutoSize = true;
			this->radioButton8->Location = System::Drawing::Point(19, 129);
			this->radioButton8->Name = L"radioButton8";
			this->radioButton8->Size = System::Drawing::Size(86, 20);
			this->radioButton8->TabIndex = 16;
			this->radioButton8->TabStop = true;
			this->radioButton8->Text = L"Align by L";
			this->radioButton8->UseVisualStyleBackColor = true;
			this->radioButton8->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton8_CheckedChanged);
			// 
			// textBox10
			// 
			this->textBox10->Location = System::Drawing::Point(12, 180);
			this->textBox10->Name = L"textBox10";
			this->textBox10->Size = System::Drawing::Size(100, 22);
			this->textBox10->TabIndex = 20;
			// 
			// trackBar1
			// 
			this->trackBar1->Location = System::Drawing::Point(117, 23);
			this->trackBar1->Maximum = 99;
			this->trackBar1->Minimum = 1;
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(104, 56);
			this->trackBar1->TabIndex = 17;
			this->trackBar1->Value = 50;
			this->trackBar1->ValueChanged += gcnew System::EventHandler(this, &Form1::trackbar1_changed);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(218, 34);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(63, 22);
			this->textBox1->TabIndex = 18;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(134, 59);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(147, 16);
			this->label1->TabIndex = 19;
			this->label1->Text = L"Star detection threshold";
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->radioButton4);
			this->panel1->Controls->Add(this->radioButton3);
			this->panel1->Controls->Add(this->radioButton2);
			this->panel1->Controls->Add(this->radioButton1);
			this->panel1->Location = System::Drawing::Point(339, 10);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(100, 157);
			this->panel1->TabIndex = 21;
			// 
			// panel2
			// 
			this->panel2->Controls->Add(this->radioButton8);
			this->panel2->Controls->Add(this->radioButton7);
			this->panel2->Controls->Add(this->radioButton6);
			this->panel2->Controls->Add(this->radioButton5);
			this->panel2->Location = System::Drawing::Point(459, 10);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(131, 156);
			this->panel2->TabIndex = 22;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(600, 300);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->trackBar1);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->textBox10);
			this->Controls->Add(this->textBox1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar1))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		int k = ReadImages();
		textBox10->Clear();
		textBox10->AppendText("Milliseconds: " + k.ToString());
	}
	
	private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
		int k = ComputeOffsets();
		textBox10->Clear();
		textBox10->AppendText("Milliseconds: " + k.ToString());
	}

	private: System::Void button3_Click(System::Object^ sender, System::EventArgs^ e) {
		int k = Stack();
		textBox10->Clear();
		textBox10->AppendText("Milliseconds: " + k.ToString());
	}

	private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e) {

	}

	private: System::Void textBox1_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	}
	
	private: System::Void radioButton1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		filter = "R";
	}

	private: System::Void radioButton2_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		filter = "G";
	}

	private: System::Void radioButton3_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		filter = "B";
	}

	private: System::Void radioButton4_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		filter = "L";
	}

	private: System::Void radioButton5_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		align = "R";
	}

	private: System::Void radioButton6_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		align = "G";
	}

	private: System::Void radioButton7_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		align = "B";
	}

	private: System::Void radioButton8_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
		align = "L";
	}
	private: System::Void trackbar1_changed(System::Object^ sender, System::EventArgs^ e) {
		textBox1->Clear();
		int k = (trackBar1->Value);
		textBox1->Text = String::Concat("", k);
		detectionThreshold = float(k)/100;
	}
};
}
