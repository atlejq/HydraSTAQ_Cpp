#pragma once
#include <stdlib.h>     /* srand, rand */
#include <stdexcept>

extern std::string path;
extern std::string parameterDir;
extern std::string outputDir;
extern std::string lightDir;
extern std::string darkDir;
extern std::string flatDir;
extern std::string flatDarksDir;
extern std::string darksGroup;
extern std::string flatDarksGroup;
extern std::string ext;
extern std::string frameFilter;
extern std::string alignFilter;
extern int detectionThreshold;
extern int discardPercentage;
extern int medianBatchSize;
extern int interpolationFlag;
extern int maxStars;
extern int topMatches;
extern int numLogicalCores;
extern float samplingFactor;

namespace Hydra {
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

		public:std::vector<int> RegisterFrames();

		public:std::vector<int> ComputeOffsets();

		public:std::vector<int> Stack();

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
		private: System::Windows::Forms::FolderBrowserDialog^ folderBrowserDialog1;

		private: System::Windows::Forms::RadioButton^ radioButton1;
		private: System::Windows::Forms::RadioButton^ radioButton2;
		private: System::Windows::Forms::RadioButton^ radioButton3;
		private: System::Windows::Forms::RadioButton^ radioButton4;
		private: System::Windows::Forms::RadioButton^ radioButton5;
		private: System::Windows::Forms::RadioButton^ radioButton6;
		private: System::Windows::Forms::RadioButton^ radioButton7;
		private: System::Windows::Forms::RadioButton^ radioButton8;
		private: System::Windows::Forms::RadioButton^ radioButton9;
		private: System::Windows::Forms::RadioButton^ radioButton10;
		private: System::Windows::Forms::RadioButton^ radioButton11;
		private: System::Windows::Forms::RadioButton^ radioButton12;
		private: System::Windows::Forms::RadioButton^ radioButton13;
		private: System::Windows::Forms::RadioButton^ radioButton14;
		private: System::Windows::Forms::RadioButton^ radioButton15;
		private: System::Windows::Forms::RadioButton^ radioButton16;
		private: System::Windows::Forms::RadioButton^ radioButton17;
		private: System::Windows::Forms::RadioButton^ radioButton18;
		private: System::Windows::Forms::RadioButton^ radioButton19;
		private: System::Windows::Forms::RadioButton^ radioButton20;

		private: System::Windows::Forms::TextBox^ textBox1;

		private: System::Windows::Forms::Label^ label1;
		private: System::Windows::Forms::Label^ label2;
		private: System::Windows::Forms::Label^ label3;
		private: System::Windows::Forms::Label^ label4;
		private: System::Windows::Forms::Label^ label5;
		private: System::Windows::Forms::Label^ label6;
		private: System::Windows::Forms::Label^ label7;
		private: System::Windows::Forms::Label^ label8;
		private: System::Windows::Forms::Label^ label9;
		private: System::Windows::Forms::Label^ label10;

		private: System::Windows::Forms::NumericUpDown^ numericUpDown1;
		private: System::Windows::Forms::NumericUpDown^ numericUpDown2;
		private: System::Windows::Forms::NumericUpDown^ numericUpDown3;
		private: System::Windows::Forms::NumericUpDown^ numericUpDown4;
		private: System::Windows::Forms::NumericUpDown^ numericUpDown5;

		private: System::Windows::Forms::Panel^ panel1;
		private: System::Windows::Forms::Panel^ panel2;
		private: System::Windows::Forms::Panel^ panel3;
		private: System::Windows::Forms::Panel^ panel4;
		private: System::Windows::Forms::Panel^ panel5;
		private: System::Windows::Forms::Panel^ panel6;

		protected:

		private:
			/// <summary>
			/// Required designer variable.
			/// </summary>
			System::ComponentModel::Container^ components;

	#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->button1 = (gcnew System::Windows::Forms::Button());
				this->button2 = (gcnew System::Windows::Forms::Button());
				this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
				this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton3 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton4 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton5 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton6 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton7 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton8 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton9 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton10 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton11 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton12 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton13 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton14 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton15 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton16 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton17 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton18 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton19 = (gcnew System::Windows::Forms::RadioButton());
				this->radioButton20 = (gcnew System::Windows::Forms::RadioButton());
				this->label1 = (gcnew System::Windows::Forms::Label());
				this->label2 = (gcnew System::Windows::Forms::Label());
				this->label3 = (gcnew System::Windows::Forms::Label());
				this->label4 = (gcnew System::Windows::Forms::Label());
				this->label5 = (gcnew System::Windows::Forms::Label());
				this->label6 = (gcnew System::Windows::Forms::Label());
				this->label7 = (gcnew System::Windows::Forms::Label());
				this->label8 = (gcnew System::Windows::Forms::Label());
				this->label9 = (gcnew System::Windows::Forms::Label());
				this->label10 = (gcnew System::Windows::Forms::Label());
				this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown2 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown3 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown4 = (gcnew System::Windows::Forms::NumericUpDown());
				this->numericUpDown5 = (gcnew System::Windows::Forms::NumericUpDown());
				this->panel1 = (gcnew System::Windows::Forms::Panel());
				this->panel2 = (gcnew System::Windows::Forms::Panel());
				this->panel3 = (gcnew System::Windows::Forms::Panel());
				this->panel4 = (gcnew System::Windows::Forms::Panel());
				this->panel5 = (gcnew System::Windows::Forms::Panel());
				this->panel6 = (gcnew System::Windows::Forms::Panel());
				this->textBox1 = (gcnew System::Windows::Forms::TextBox());
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown2))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown3))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown4))->BeginInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown5))->BeginInit();
				this->panel1->SuspendLayout();
				this->panel2->SuspendLayout();
				this->panel3->SuspendLayout();
				this->panel4->SuspendLayout();
				this->panel5->SuspendLayout();
				this->panel6->SuspendLayout();
				this->SuspendLayout();
				// 
				// button1
				// 
				this->button1->Location = System::Drawing::Point(20, 151);
				this->button1->Name = L"button1";
				this->button1->Size = System::Drawing::Size(74, 23);
				this->button1->TabIndex = 0;
				this->button1->Text = L"Execute";
				this->button1->UseVisualStyleBackColor = true;
				this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
				// 
				// button2
				// 
				this->button2->Location = System::Drawing::Point(20, 196);
				this->button2->Name = L"button2";
				this->button2->Size = System::Drawing::Size(75, 23);
				this->button2->TabIndex = 32;
				this->button2->Text = L"Folder";
				this->button2->UseVisualStyleBackColor = true;
				this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
				// 
				// radioButton1
				// 
				this->radioButton1->AutoSize = true;
				this->radioButton1->Checked = true;
				this->radioButton1->Location = System::Drawing::Point(13, 8);
				this->radioButton1->Name = L"radioButton1";
				this->radioButton1->Size = System::Drawing::Size(38, 20);
				this->radioButton1->TabIndex = 9;
				this->radioButton1->TabStop = true;
				this->radioButton1->Text = L"R";
				this->radioButton1->UseVisualStyleBackColor = true;
				this->radioButton1->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton1_CheckedChanged);
				// 
				// radioButton2
				// 
				this->radioButton2->AutoSize = true;
				this->radioButton2->Location = System::Drawing::Point(59, 8);
				this->radioButton2->Name = L"radioButton2";
				this->radioButton2->Size = System::Drawing::Size(38, 20);
				this->radioButton2->TabIndex = 10;
				this->radioButton2->TabStop = true;
				this->radioButton2->Text = L"G";
				this->radioButton2->UseVisualStyleBackColor = true;
				this->radioButton2->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton2_CheckedChanged);
				// 
				// radioButton3
				// 
				this->radioButton3->AutoSize = true;
				this->radioButton3->Location = System::Drawing::Point(104, 8);
				this->radioButton3->Name = L"radioButton3";
				this->radioButton3->Size = System::Drawing::Size(37, 20);
				this->radioButton3->TabIndex = 11;
				this->radioButton3->TabStop = true;
				this->radioButton3->Text = L"B";
				this->radioButton3->UseVisualStyleBackColor = true;
				this->radioButton3->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton3_CheckedChanged);
				// 
				// radioButton4
				// 
				this->radioButton4->AutoSize = true;
				this->radioButton4->Location = System::Drawing::Point(150, 8);
				this->radioButton4->Name = L"radioButton4";
				this->radioButton4->Size = System::Drawing::Size(35, 20);
				this->radioButton4->TabIndex = 12;
				this->radioButton4->TabStop = true;
				this->radioButton4->Text = L"L";
				this->radioButton4->UseVisualStyleBackColor = true;
				this->radioButton4->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton4_CheckedChanged);
				// 
				// radioButton5
				// 
				this->radioButton5->AutoSize = true;
				this->radioButton5->Checked = true;
				this->radioButton5->Enabled = false;
				this->radioButton5->Location = System::Drawing::Point(14, 7);
				this->radioButton5->Name = L"radioButton5";
				this->radioButton5->Size = System::Drawing::Size(38, 20);
				this->radioButton5->TabIndex = 13;
				this->radioButton5->TabStop = true;
				this->radioButton5->Text = L"R";
				this->radioButton5->UseVisualStyleBackColor = true;
				this->radioButton5->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton5_CheckedChanged);
				// 
				// radioButton6
				// 
				this->radioButton6->AutoSize = true;
				this->radioButton6->Enabled = false;
				this->radioButton6->Location = System::Drawing::Point(59, 7);
				this->radioButton6->Name = L"radioButton6";
				this->radioButton6->Size = System::Drawing::Size(38, 20);
				this->radioButton6->TabIndex = 14;
				this->radioButton6->TabStop = true;
				this->radioButton6->Text = L"G";
				this->radioButton6->UseVisualStyleBackColor = true;
				this->radioButton6->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton6_CheckedChanged);
				// 
				// radioButton7
				// 
				this->radioButton7->AutoSize = true;
				this->radioButton7->Enabled = false;
				this->radioButton7->Location = System::Drawing::Point(104, 7);
				this->radioButton7->Name = L"radioButton7";
				this->radioButton7->Size = System::Drawing::Size(37, 20);
				this->radioButton7->TabIndex = 15;
				this->radioButton7->TabStop = true;
				this->radioButton7->Text = L"B";
				this->radioButton7->UseVisualStyleBackColor = true;
				this->radioButton7->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton7_CheckedChanged);
				// 
				// radioButton8
				// 
				this->radioButton8->AutoSize = true;
				this->radioButton8->Enabled = false;
				this->radioButton8->Location = System::Drawing::Point(149, 7);
				this->radioButton8->Name = L"radioButton8";
				this->radioButton8->Size = System::Drawing::Size(35, 20);
				this->radioButton8->TabIndex = 16;
				this->radioButton8->TabStop = true;
				this->radioButton8->Text = L"L";
				this->radioButton8->UseVisualStyleBackColor = true;
				this->radioButton8->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton8_CheckedChanged);
				// 
				// radioButton9
				// 
				this->radioButton9->AutoSize = true;
				this->radioButton9->Checked = true;
				this->radioButton9->Location = System::Drawing::Point(8, 9);
				this->radioButton9->Name = L"radioButton9";
				this->radioButton9->Size = System::Drawing::Size(79, 20);
				this->radioButton9->TabIndex = 28;
				this->radioButton9->TabStop = true;
				this->radioButton9->Text = L"Register";
				this->radioButton9->UseVisualStyleBackColor = true;
				this->radioButton9->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton9_CheckedChanged);
				// 
				// radioButton10
				// 
				this->radioButton10->AutoSize = true;
				this->radioButton10->Location = System::Drawing::Point(8, 53);
				this->radioButton10->Name = L"radioButton10";
				this->radioButton10->Size = System::Drawing::Size(69, 20);
				this->radioButton10->TabIndex = 29;
				this->radioButton10->TabStop = true;
				this->radioButton10->Text = L"Offsets";
				this->radioButton10->UseVisualStyleBackColor = true;
				this->radioButton10->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton10_CheckedChanged);
				// 
				// radioButton11
				// 
				this->radioButton11->AutoSize = true;
				this->radioButton11->Location = System::Drawing::Point(8, 98);
				this->radioButton11->Name = L"radioButton11";
				this->radioButton11->Size = System::Drawing::Size(62, 20);
				this->radioButton11->TabIndex = 30;
				this->radioButton11->TabStop = true;
				this->radioButton11->Text = L"Stack";
				this->radioButton11->UseVisualStyleBackColor = true;
				this->radioButton11->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton11_CheckedChanged);
				// 
				// radioButton12
				// 
				this->radioButton12->AutoSize = true;
				this->radioButton12->Enabled = false;
				this->radioButton12->Location = System::Drawing::Point(15, 3);
				this->radioButton12->Name = L"radioButton12";
				this->radioButton12->Size = System::Drawing::Size(67, 20);
				this->radioButton12->TabIndex = 33;
				this->radioButton12->TabStop = true;
				this->radioButton12->Text = L"LRGB ";
				this->radioButton12->UseVisualStyleBackColor = true;
				this->radioButton12->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton12_CheckedChanged);
				// 
				// radioButton13
				// 
				this->radioButton13->AutoSize = true;
				this->radioButton13->Checked = true;
				this->radioButton13->Enabled = false;
				this->radioButton13->Location = System::Drawing::Point(94, 3);
				this->radioButton13->Name = L"radioButton13";
				this->radioButton13->Size = System::Drawing::Size(77, 20);
				this->radioButton13->TabIndex = 34;
				this->radioButton13->TabStop = true;
				this->radioButton13->Text = L"L + RGB";
				this->radioButton13->UseVisualStyleBackColor = true;
				this->radioButton13->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton13_CheckedChanged);
				// 
				// radioButton14
				// 
				this->radioButton14->AutoSize = true;
				this->radioButton14->Enabled = false;
				this->radioButton14->Location = System::Drawing::Point(182, 3);
				this->radioButton14->Name = L"radioButton14";
				this->radioButton14->Size = System::Drawing::Size(68, 20);
				this->radioButton14->TabIndex = 35;
				this->radioButton14->Text = L"Pr filter";
				this->radioButton14->UseVisualStyleBackColor = true;
				this->radioButton14->CheckedChanged += gcnew System::EventHandler(this, &Form1::radioButton14_CheckedChanged);
				// 
				// radioButton15
				// 
				this->radioButton15->AutoSize = true;
				this->radioButton15->Checked = true;
				this->radioButton15->Enabled = false;
				this->radioButton15->Location = System::Drawing::Point(15, 3);
				this->radioButton15->Name = L"radioButton15";
				this->radioButton15->Size = System::Drawing::Size(67, 20);
				this->radioButton15->TabIndex = 33;
				this->radioButton15->TabStop = true;
				this->radioButton15->Text = L"LRGB ";
				this->radioButton15->UseVisualStyleBackColor = true;
				// 
				// radioButton16
				// 
				this->radioButton16->AutoSize = true;
				this->radioButton16->Enabled = false;
				this->radioButton16->Location = System::Drawing::Point(94, 3);
				this->radioButton16->Name = L"radioButton16";
				this->radioButton16->Size = System::Drawing::Size(77, 20);
				this->radioButton16->TabIndex = 34;
				this->radioButton16->Text = L"L + RGB";
				this->radioButton16->UseVisualStyleBackColor = true;
				// 
				// radioButton17
				// 
				this->radioButton17->AutoSize = true;
				this->radioButton17->Enabled = false;
				this->radioButton17->Location = System::Drawing::Point(182, 3);
				this->radioButton17->Name = L"radioButton17";
				this->radioButton17->Size = System::Drawing::Size(68, 20);
				this->radioButton17->TabIndex = 35;
				this->radioButton17->Text = L"Pr filter";
				this->radioButton17->UseVisualStyleBackColor = true;
				// 
				// radioButton18
				// 
				this->radioButton18->AutoSize = true;
				this->radioButton18->Enabled = false;
				this->radioButton18->Location = System::Drawing::Point(14, 4);
				this->radioButton18->Name = L"radioButton18";
				this->radioButton18->Size = System::Drawing::Size(48, 20);
				this->radioButton18->TabIndex = 0;
				this->radioButton18->TabStop = true;
				this->radioButton18->Text = L"NN";
				this->radioButton18->UseVisualStyleBackColor = true;
				// 
				// radioButton19
				// 
				this->radioButton19->AutoSize = true;
				this->radioButton19->Enabled = false;
				this->radioButton19->Location = System::Drawing::Point(68, 4);
				this->radioButton19->Name = L"radioButton19";
				this->radioButton19->Size = System::Drawing::Size(73, 20);
				this->radioButton19->TabIndex = 1;
				this->radioButton19->TabStop = true;
				this->radioButton19->Text = L"Bilinear";
				this->radioButton19->UseVisualStyleBackColor = true;
				// 
				// radioButton20
				// 
				this->radioButton20->AutoSize = true;
				this->radioButton20->Checked = true;
				this->radioButton20->Enabled = false;
				this->radioButton20->Location = System::Drawing::Point(147, 4);
				this->radioButton20->Name = L"radioButton20";
				this->radioButton20->Size = System::Drawing::Size(72, 20);
				this->radioButton20->TabIndex = 2;
				this->radioButton20->TabStop = true;
				this->radioButton20->Text = L"Bicubic";
				this->radioButton20->UseVisualStyleBackColor = true;
				// 
				// label1
				// 
				this->label1->AutoSize = true;
				this->label1->Location = System::Drawing::Point(136, 20);
				this->label1->Name = L"label1";
				this->label1->Size = System::Drawing::Size(147, 16);
				this->label1->TabIndex = 19;
				this->label1->Text = L"Star detection threshold";
				// 
				// label2
				// 
				this->label2->AutoSize = true;
				this->label2->Location = System::Drawing::Point(136, 64);
				this->label2->Name = L"label2";
				this->label2->Size = System::Drawing::Size(69, 16);
				this->label2->TabIndex = 24;
				this->label2->Text = L"Align stars";
				// 
				// label3
				// 
				this->label3->AutoSize = true;
				this->label3->Location = System::Drawing::Point(136, 110);
				this->label3->Name = L"label3";
				this->label3->Size = System::Drawing::Size(126, 16);
				this->label3->TabIndex = 27;
				this->label3->Text = L"Discard percentage";
				// 
				// label4
				// 
				this->label4->AutoSize = true;
				this->label4->Location = System::Drawing::Point(413, 156);
				this->label4->Name = L"label4";
				this->label4->Size = System::Drawing::Size(81, 16);
				this->label4->TabIndex = 36;
				this->label4->Text = L"Group darks";
				// 
				// label5
				// 
				this->label5->AutoSize = true;
				this->label5->Location = System::Drawing::Point(413, 200);
				this->label5->Name = L"label5";
				this->label5->Size = System::Drawing::Size(98, 16);
				this->label5->TabIndex = 38;
				this->label5->Text = L"Group flatdarks";
				// 
				// label6
				// 
				this->label6->AutoSize = true;
				this->label6->Location = System::Drawing::Point(136, 154);
				this->label6->Name = L"label6";
				this->label6->Size = System::Drawing::Size(115, 16);
				this->label6->TabIndex = 40;
				this->label6->Text = L"Median batch size";
				// 
				// label7
				// 
				this->label7->AutoSize = true;
				this->label7->Location = System::Drawing::Point(413, 111);
				this->label7->Name = L"label7";
				this->label7->Size = System::Drawing::Size(80, 16);
				this->label7->TabIndex = 41;
				this->label7->Text = L"Interpolation";
				// 
				// label8
				// 
				this->label8->AutoSize = true;
				this->label8->Location = System::Drawing::Point(136, 200);
				this->label8->Name = L"label8";
				this->label8->Size = System::Drawing::Size(47, 16);
				this->label8->TabIndex = 43;
				this->label8->Text = L"Drizzle";
				// 
				// label9
				// 
				this->label9->AutoSize = true;
				this->label9->Location = System::Drawing::Point(413, 20);
				this->label9->Name = L"label9";
				this->label9->Size = System::Drawing::Size(57, 16);
				this->label9->TabIndex = 44;
				this->label9->Text = L"Process";
				// 
				// label10
				// 
				this->label10->AutoSize = true;
				this->label10->Location = System::Drawing::Point(413, 65);
				this->label10->Name = L"label10";
				this->label10->Size = System::Drawing::Size(55, 16);
				this->label10->TabIndex = 45;
				this->label10->Text = L"Align by";
				// 
				// numericUpDown1
				// 
				this->numericUpDown1->Location = System::Drawing::Point(304, 17);
				this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 99, 0, 0, 0 });
				this->numericUpDown1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
				this->numericUpDown1->Name = L"numericUpDown1";
				this->numericUpDown1->Size = System::Drawing::Size(66, 22);
				this->numericUpDown1->TabIndex = 23;
				this->numericUpDown1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 50, 0, 0, 0 });
				// 
				// numericUpDown2
				// 
				this->numericUpDown2->Enabled = false;
				this->numericUpDown2->Location = System::Drawing::Point(304, 62);
				this->numericUpDown2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 12, 0, 0, 0 });
				this->numericUpDown2->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 6, 0, 0, 0 });
				this->numericUpDown2->Name = L"numericUpDown2";
				this->numericUpDown2->Size = System::Drawing::Size(66, 22);
				this->numericUpDown2->TabIndex = 25;
				this->numericUpDown2->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 6, 0, 0, 0 });
				// 
				// numericUpDown3
				// 
				this->numericUpDown3->Enabled = false;
				this->numericUpDown3->Location = System::Drawing::Point(304, 107);
				this->numericUpDown3->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 99, 0, 0, 0 });
				this->numericUpDown3->Name = L"numericUpDown3";
				this->numericUpDown3->Size = System::Drawing::Size(66, 22);
				this->numericUpDown3->TabIndex = 26;
				this->numericUpDown3->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
				// 
				// numericUpDown4
				// 
				this->numericUpDown4->Enabled = false;
				this->numericUpDown4->Location = System::Drawing::Point(304, 152);
				this->numericUpDown4->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 50, 0, 0, 0 });
				this->numericUpDown4->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 0 });
				this->numericUpDown4->Name = L"numericUpDown4";
				this->numericUpDown4->Size = System::Drawing::Size(66, 22);
				this->numericUpDown4->TabIndex = 39;
				this->numericUpDown4->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 30, 0, 0, 0 });
				// 
				// numericUpDown5
				// 
				this->numericUpDown5->DecimalPlaces = 2;
				this->numericUpDown5->Enabled = false;
				this->numericUpDown5->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 65536 });
				this->numericUpDown5->Location = System::Drawing::Point(304, 197);
				this->numericUpDown5->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 2, 0, 0, 0 });
				this->numericUpDown5->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 5, 0, 0, 65536 });
				this->numericUpDown5->Name = L"numericUpDown5";
				this->numericUpDown5->Size = System::Drawing::Size(66, 22);
				this->numericUpDown5->TabIndex = 42;
				this->numericUpDown5->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 10, 0, 0, 65536 });
				// 
				// panel1
				// 
				this->panel1->Controls->Add(this->radioButton1);
				this->panel1->Controls->Add(this->radioButton2);
				this->panel1->Controls->Add(this->radioButton3);
				this->panel1->Controls->Add(this->radioButton4);
				this->panel1->Location = System::Drawing::Point(543, 11);
				this->panel1->Name = L"panel1";
				this->panel1->Size = System::Drawing::Size(192, 35);
				this->panel1->TabIndex = 21;
				// 
				// panel2
				// 
				this->panel2->Controls->Add(this->radioButton5);
				this->panel2->Controls->Add(this->radioButton6);
				this->panel2->Controls->Add(this->radioButton7);
				this->panel2->Controls->Add(this->radioButton8);
				this->panel2->Location = System::Drawing::Point(543, 57);
				this->panel2->Name = L"panel2";
				this->panel2->Size = System::Drawing::Size(192, 34);
				this->panel2->TabIndex = 22;
				// 
				// panel3
				// 
				this->panel3->Controls->Add(this->radioButton9);
				this->panel3->Controls->Add(this->radioButton10);
				this->panel3->Controls->Add(this->radioButton11);
				this->panel3->Location = System::Drawing::Point(12, 10);
				this->panel3->Name = L"panel3";
				this->panel3->Size = System::Drawing::Size(92, 130);
				this->panel3->TabIndex = 31;
				// 
				// panel4
				// 
				this->panel4->Controls->Add(this->radioButton12);
				this->panel4->Controls->Add(this->radioButton13);
				this->panel4->Controls->Add(this->radioButton14);
				this->panel4->Location = System::Drawing::Point(543, 152);
				this->panel4->Name = L"panel4";
				this->panel4->Size = System::Drawing::Size(253, 26);
				this->panel4->TabIndex = 35;
				// 
				// panel5
				// 
				this->panel5->Controls->Add(this->radioButton15);
				this->panel5->Controls->Add(this->radioButton16);
				this->panel5->Controls->Add(this->radioButton17);
				this->panel5->Location = System::Drawing::Point(543, 195);
				this->panel5->Name = L"panel5";
				this->panel5->Size = System::Drawing::Size(253, 27);
				this->panel5->TabIndex = 37;
				// 
				// panel6
				// 
				this->panel6->Controls->Add(this->radioButton18);
				this->panel6->Controls->Add(this->radioButton19);
				this->panel6->Controls->Add(this->radioButton20);
				this->panel6->Location = System::Drawing::Point(543, 104);
				this->panel6->Name = L"panel6";
				this->panel6->Size = System::Drawing::Size(224, 28);
				this->panel6->TabIndex = 36;
				// 
				// textBox1
				// 
				this->textBox1->Location = System::Drawing::Point(137, 249);
				this->textBox1->Name = L"textBox1";
				this->textBox1->Size = System::Drawing::Size(502, 22);
				this->textBox1->TabIndex = 20;
				// 
				// Form1
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(821, 284);
				this->Controls->Add(this->button1);
				this->Controls->Add(this->button2);
				this->Controls->Add(this->textBox1);
				this->Controls->Add(this->numericUpDown1);
				this->Controls->Add(this->numericUpDown2);
				this->Controls->Add(this->numericUpDown3);
				this->Controls->Add(this->numericUpDown4);
				this->Controls->Add(this->numericUpDown5);
				this->Controls->Add(this->label1);
				this->Controls->Add(this->label2);
				this->Controls->Add(this->label3);
				this->Controls->Add(this->label4);
				this->Controls->Add(this->label5);
				this->Controls->Add(this->label6);
				this->Controls->Add(this->label7);
				this->Controls->Add(this->label8);
				this->Controls->Add(this->label9);
				this->Controls->Add(this->label10);
				this->Controls->Add(this->panel1);
				this->Controls->Add(this->panel2);
				this->Controls->Add(this->panel3);
				this->Controls->Add(this->panel4);
				this->Controls->Add(this->panel5);
				this->Controls->Add(this->panel6);
				this->Name = L"Form1";
				this->Text = L"HydraSTAQ";
				this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown2))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown3))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown4))->EndInit();
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown5))->EndInit();
				this->panel1->ResumeLayout(false);
				this->panel1->PerformLayout();
				this->panel2->ResumeLayout(false);
				this->panel2->PerformLayout();
				this->panel3->ResumeLayout(false);
				this->panel3->PerformLayout();
				this->panel4->ResumeLayout(false);
				this->panel4->PerformLayout();
				this->panel5->ResumeLayout(false);
				this->panel5->PerformLayout();
				this->panel6->ResumeLayout(false);
				this->panel6->PerformLayout();
				this->ResumeLayout(false);
				this->PerformLayout();

			}
	#pragma endregion
		private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
			if (path == "")
			{
				textBox1->Clear();
				textBox1->AppendText("Choose a folder.");
			}
			else
			{
				if ((radioButton9->Checked))
				{
					detectionThreshold = int((numericUpDown1->Value));
					std::vector<int> k = RegisterFrames();
					textBox1->Clear();
					if (k[0] == 0)
						textBox1->AppendText("No light frames found in this folder.");
					else
						textBox1->AppendText("Registered " + k[0].ToString() + " frames in " + k[1].ToString() + " milliseconds.");
				}

				if ((radioButton10->Checked))
				{
					topMatches = int((numericUpDown2->Value));
					discardPercentage = int((numericUpDown3->Value));
					std::vector<int> k = ComputeOffsets();
					textBox1->Clear();
					if (k[0] == 0)
						textBox1->AppendText("No frame registration data found in this folder.");
					else
						textBox1->AppendText("Computed offsets for " + k[0].ToString() + " frames in " + k[1].ToString() + " milliseconds.");
				}

				if ((radioButton11->Checked))
				{
					medianBatchSize = int((numericUpDown4->Value));
					samplingFactor = float((numericUpDown5->Value));
					std::vector<int> k = Stack();
					textBox1->Clear();
					if (k[0] == 0)
						textBox1->AppendText("No offsets for frame alignment found in this folder.");
					else
						textBox1->AppendText("Stacked " + k[0].ToString() + " frames in " + k[1].ToString() + " milliseconds.");
				}
			}
		}

		private: System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e) {
		}

		private: System::Void radioButton1_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			frameFilter = "R";
		}

		private: System::Void radioButton2_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			frameFilter = "G";
		}

		private: System::Void radioButton3_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			frameFilter = "B";
		}

		private: System::Void radioButton4_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			frameFilter = "L";
		}

		private: System::Void radioButton5_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			alignFilter = "R";
		}

		private: System::Void radioButton6_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			alignFilter = "G";
		}

		private: System::Void radioButton7_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			alignFilter = "B";
		}

		private: System::Void radioButton8_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			alignFilter = "L";
		}

		private: System::Void radioButton9_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			numericUpDown1->Enabled = true;
			numericUpDown2->Enabled = false;
			numericUpDown3->Enabled = false;
			numericUpDown4->Enabled = false;
			numericUpDown5->Enabled = false;
			radioButton5->Enabled = false;
			radioButton6->Enabled = false;
			radioButton7->Enabled = false;
			radioButton8->Enabled = false;
			radioButton12->Enabled = false;
			radioButton13->Enabled = false;
			radioButton14->Enabled = false;
			radioButton15->Enabled = false;
			radioButton16->Enabled = false;
			radioButton17->Enabled = false;
			radioButton18->Enabled = false;
			radioButton19->Enabled = false;
			radioButton20->Enabled = false;
		}

		private: System::Void radioButton10_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			numericUpDown1->Enabled = false;
			numericUpDown2->Enabled = true;
			numericUpDown3->Enabled = true;
			numericUpDown4->Enabled = false;
			numericUpDown5->Enabled = false;
			radioButton5->Enabled = true;
			radioButton6->Enabled = true;
			radioButton7->Enabled = true;
			radioButton8->Enabled = true;
			radioButton12->Enabled = false;
			radioButton13->Enabled = false;
			radioButton14->Enabled = false;
			radioButton15->Enabled = false;
			radioButton16->Enabled = false;
			radioButton17->Enabled = false;
			radioButton18->Enabled = false;
			radioButton19->Enabled = false;
			radioButton20->Enabled = false;
		}

		private: System::Void radioButton11_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			numericUpDown1->Enabled = false;
			numericUpDown2->Enabled = false;
			numericUpDown3->Enabled = false;
			numericUpDown4->Enabled = true;
			numericUpDown5->Enabled = true;
			radioButton5->Enabled = false;
			radioButton6->Enabled = false;
			radioButton7->Enabled = false;
			radioButton8->Enabled = false;
			radioButton12->Enabled = true;
			radioButton13->Enabled = true;
			radioButton14->Enabled = true;
			radioButton15->Enabled = true;
			radioButton16->Enabled = true;
			radioButton17->Enabled = true;
			radioButton18->Enabled = true;
			radioButton19->Enabled = true;
			radioButton20->Enabled = true;
		}

		private: System::Void radioButton12_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			darksGroup = "LRGB";
		}

		private: System::Void radioButton13_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			darksGroup = "RGB";
		}

		private: System::Void radioButton14_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			darksGroup = frameFilter;
		}

		private: System::Void radioButton15_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			flatDarksGroup = "LRGB";
		}

		private: System::Void radioButton16_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			flatDarksGroup = "RGB";
		}

		private: System::Void radioButton17_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			flatDarksGroup = frameFilter;
		}

		private: System::Void radioButton18_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			interpolationFlag = 0;
		}

		private: System::Void radioButton19_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			interpolationFlag = 1;
		}

		private: System::Void radioButton20_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
			interpolationFlag = 2;
		}

		private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) {
			if (folderBrowserDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				textBox1->Text = folderBrowserDialog1->SelectedPath;
				std::string os;
				MarshalString((folderBrowserDialog1->SelectedPath), os);
				path = os;
			}
		}

		void MarshalString(System::String^ s, std::string& os) {
				   using namespace Runtime::InteropServices;
				   const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
				   os = chars;
				   Marshal::FreeHGlobal(IntPtr((void*)chars));
		}
	};
}