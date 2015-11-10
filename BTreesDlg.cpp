
// BTreesDlg.cpp : file di implementazione
//

#include "stdafx.h"
#include "BTrees.h"
#include "BTreesDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// finestra di dialogo CAboutDlg utilizzata per visualizzare le informazioni sull'applicazione.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // supporto DDX/DDV

// Implementazione
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// finestra di dialogo CBTreesDlg



CBTreesDlg::CBTreesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBTreesDlg::IDD, pParent)
	, BTreeDegreeNumber(false)
	, BTree(NULL)
	, BTreeChangeDegreeEnd(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBTreesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTREE, BTreeView);
	DDX_Control(pDX, IDC_BTREEDEGREEEDIT, BTreeDegree);
}

BEGIN_MESSAGE_MAP(CBTreesDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_LOADBUTTON, &CBTreesDlg::OnBnClickedLoadbutton)
	ON_BN_CLICKED(IDC_CHANGEDEGREEBUTTON, &CBTreesDlg::OnBnClickedChangedegreebutton)
END_MESSAGE_MAP()


// gestori di messaggi di CBTreesDlg

BOOL CBTreesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Aggiungere la voce di menu "Informazioni su..." al menu di sistema.

	// IDM_ABOUTBOX deve trovarsi tra i comandi di sistema.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Impostare l'icona per questa finestra di dialogo.  Il framework non esegue questa operazione automaticamente
	//  se la finestra principale dell'applicazione non è una finestra di dialogo.
	SetIcon(m_hIcon, TRUE);			// Impostare icona grande.
	SetIcon(m_hIcon, FALSE);		// Impostare icona piccola.

	GetClientRect(&BTreesDlgRect);
	GotoDlgCtrl(&BTreeDegree);

	return FALSE;  // restituisce TRUE a meno che non venga impostato lo stato attivo su un controllo.
}

void CBTreesDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// Se si aggiunge alla finestra di dialogo un pulsante di riduzione a icona, per trascinare l'icona sarà necessario
//  il codice sottostante.  Per le applicazioni MFC che utilizzano il modello documento/visualizzazione,
//  questa operazione viene eseguita automaticamente dal framework.

void CBTreesDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // contesto di dispositivo per il disegno

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrare l'icona nel rettangolo client.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Disegnare l'icona
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Il sistema chiama questa funzione per ottenere la visualizzazione del cursore durante il trascinamento
//  della finestra ridotta a icona.
HCURSOR CBTreesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// This function divides the child node y of a node x at position i into two new
// child nodes with one half of the keys and one half of the child nodes (if the old child node y is not a leaf).
TypeNode* CBTreesDlg::BTreeSplitChild(TypeNode* x, int i, TypeNode* y)
{
	// New nodes for cleaning the RAM, executed by the garbage collector
	TypeNode* aux_x;
	TypeNode* aux_y;
	// This is the new child node that is the second half of the old child node y
	TypeNode* z;
	// Index for general purpose
	int j;


	// Creation of a child node. This node will be the second half of the old child node y
	z = new TypeNode();
	// Creation of a child node. This node will be the first half of the old child node y. This node is created for cleaning of the RAM (the old child node y is greater and is destroyed)
	aux_y = new TypeNode();
	// Setting of the number of the keys (first node)
	aux_y->n = y->n / 2;
	// Setting of the number of the keys (second node). If the old child node y has an even number of the keys then the second node must have the number of the keys decreased by one. This is because in the both cases one key must be put between the new child nodes
	if (2 * aux_y->n == y->n)
		z->n = aux_y->n - 1;
	else
		z->n = aux_y->n;
	z->key = new int[z->n];
	z->c = new TypeNode*[z->n + 1];
	z->height = y->height;
	aux_y->key = new int[aux_y->n];
	aux_y->c = new TypeNode*[aux_y->n + 1];
	aux_y->height = y->height;
	// Adding of the first half of the keys into one of two new child nodes
	for (j = 0; j < aux_y->n; j++)
		aux_y->key[j] = y->key[j];
	// Adding of the second half of the keys into the other new child node
	for (j = 0; j < z->n; j++)
		z->key[j] = y->key[aux_y->n + 1 + j];
	// If the old child node y is not a leaf then all the child nodes of the node y are added into the new child nodes
	if (y->height > 0)
	{
		for (j = 0; j < aux_y->n + 1; j++)
			aux_y->c[j] = y->c[j];
		for (j = 0; j < z->n + 1; j++)
			z->c[j] = y->c[aux_y->n + 1 + j];
	}

	// Creation of the new node x that have the new child nodes as the child nodes at the i and i + 1 positions
	aux_x = new TypeNode();
	// Setting of the height : equal to node x
	aux_x->height = x->height;
	// Of course the new node x has the number of child nodes as the node x increased by one
	aux_x->c = new TypeNode*[x->n + 2];
	// Adding of the child nodes
	for (j = 0; j < i; j++)
		aux_x->c[j] = x->c[j];
	// Adding of the first new child node at the position i
	aux_x->c[i] = aux_y;
	// Adding of the second new child node at the position i + 1
	aux_x->c[i + 1] = z;
	// Completing of the child nodes
	for (j = i + 1; j < x->n + 1; j++)
		aux_x->c[j + 1] = x->c[j];
	// Adding of the keys
	aux_x->key = new int[x->n + 1];
	for (j = 0; j < i; j++)
		aux_x->key[j] = x->key[j];
	// Transfer of the key from the middle of the old child node y to the i position of the node x (between the new child nodes)
	aux_x->key[i] = y->key[aux_y->n];
	for (j = i; j < x->n; j++)
		aux_x->key[j + 1] = x->key[j];
	// Setting of the number of keys
	aux_x->n = x->n + 1;

	// Cleaning of the RAM (destroying the old nodes)
	delete(y);
	delete(x);
	// End of function (the result is the new node x)
	return aux_x;
}


TypeNode* CBTreesDlg::BTreeInsertNonFull(TypeNode* x, int k)
{
	TypeNode* aux_x;
	TypeNode* c;
	int i;


	if (x->height == 0)
	{
		aux_x = new TypeNode();
		aux_x->height = 0;
		aux_x->key = new int[x->n + 1];

		for (i = 0; i < (x->n - 1) && k > x->key[i]; i++)
			aux_x->key[i] = x->key[i];
		if (i == x->n - 1)
		{
			if (k > x->key[i])
			{
				aux_x->key[i] = x->key[i];
				aux_x->key[x->n] = k;
			}
			else
			{
				aux_x->key[i] = k;
				aux_x->key[x->n] = x->key[i];
			}
		}
		else
		{
			aux_x->key[i] = k;

			for (; i < x->n; i++)
				aux_x->key[i + 1] = x->key[i];
		}
		aux_x->n = x->n + 1;

		delete(x);
	}
	else
	{
		for (i = 0; i < (x->n - 1) && k > x->key[i]; i++);
		if (i == x->n - 1)
			if (k > x->key[i])
				i++;
		c = x->c[i];

		if (c->n == BTreeDegreeNumber * 2 - 1)
		{
			aux_x = BTreeSplitChild(x, i, c);
			if (k < aux_x->key[i])
				aux_x->c[i] = BTreeInsertNonFull(aux_x->c[i], k);
			else
				aux_x->c[i + 1] = BTreeInsertNonFull(aux_x->c[i + 1], k);
		}
		else
		{
			aux_x = x;
			aux_x->c[i] = BTreeInsertNonFull(c, k);
		}
	}

	return aux_x;
}


void CBTreesDlg::BTreeInsert(TypeNode* r, int k)
{
	TypeNode* s;


	if (r == NULL)
	{
		r = new TypeNode();
		BTree = r;
		r->height = 0;
		r->key = new int[1];
		r->key[0] = k;
		r->n = 1;
	}
	else
	{
		if (r->n == BTreeDegreeNumber * 2 - 1)
		{
			s = new TypeNode();
			s->height = r->height + 1;
			s->n = 0;
			s->c = new TypeNode*[1];
			s->c[0] = r;
			BTree = BTreeSplitChild(s, 0, r);
			BTree = BTreeInsertNonFull(BTree, k);
		}
		else
			BTree = BTreeInsertNonFull(r, k);
	}
}


void CBTreesDlg::BTreeVisualization(TypeNode *BTreeNode, HTREEITEM BTreeParent)
{
	char TextBuffer[10];
	CString *BTreeChildText;
	HTREEITEM BTreeChild;


	if (BTreeNode == NULL)
		return;

	if (BTreeParent == NULL)
	{
		BTreeView.DeleteAllItems();
		BTreeChildText = new CString("ROOT");
		BTreeParent = BTreeView.InsertItem(TVIF_TEXT | TVIF_STATE,
			BTreeChildText->GetBuffer(), 0, 0, TVIS_EXPANDED, TVIS_EXPANDED, 0,
			NULL, NULL);
	}

	for (int i = 0; i < BTreeNode->n; i++)
	{
		if (BTreeNode->height > 0)
		{
			sprintf_s(TextBuffer, "ST%d", (i + 1));
			BTreeChildText = new CString(TextBuffer);
			BTreeChild = BTreeView.InsertItem(TVIF_TEXT | TVIF_STATE,
				BTreeChildText->GetBuffer(), 0, 0, TVIS_EXPANDED, TVIS_EXPANDED, 0,
				BTreeParent, NULL);
			BTreeVisualization(BTreeNode->c[i], BTreeChild);
		}
		sprintf_s(TextBuffer, "%d", BTreeNode->key[i]);
		BTreeChildText = new CString(TextBuffer);
		BTreeChild = BTreeView.InsertItem(TVIF_TEXT | TVIF_STATE,
			BTreeChildText->GetBuffer(), 0, 0, TVIS_EXPANDED, TVIS_EXPANDED, 0,
			BTreeParent, NULL);
	}
	if (BTreeNode->height > 0)
	{
		sprintf_s(TextBuffer, "ST%d", (BTreeNode->n + 1));
		BTreeChildText = new CString(TextBuffer);
		BTreeChild = BTreeView.InsertItem(TVIF_TEXT | TVIF_STATE,
			BTreeChildText->GetBuffer(), 0, 0, TVIS_EXPANDED, TVIS_EXPANDED, 0,
			BTreeParent, NULL);
		BTreeVisualization(BTreeNode->c[BTreeNode->n], BTreeChild);
	}
}

void CBTreesDlg::BTreeElimination(TypeNode *BTreeNode)
{
	if (BTreeNode == NULL)
		return;

	if (BTreeNode->height > 0)
		for (int i = 0; i <= BTreeNode->n; i++)
			BTreeElimination(BTreeNode->c[i]);
	delete(BTreeNode);
}

void CBTreesDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
		return;

	RECT BTreeViewRect;


	BTreeView.GetWindowRect(&BTreeViewRect);
	BTreeView.SetWindowPos(NULL, BTreeViewRect.left, BTreeViewRect.top,
		BTreeViewRect.right - BTreeViewRect.left +
		(LONG)cx - BTreesDlgRect.right + BTreesDlgRect.left,
		BTreeViewRect.bottom - BTreeViewRect.top +
		(LONG)cy - BTreesDlgRect.bottom + BTreesDlgRect.top,
		SWP_NOMOVE);
	GetClientRect(&BTreesDlgRect);
}


// This function creates a new child node of a node x. The new child node has the child nodes and the keys of the nodes y and z that are the child nodes of the node x at the position i and i + 1. The 2 child nodes are merged with the key i of the node x in the middle.
TypeNode* CBTreesDlg::BTreeMergeChilds(TypeNode* x, TypeNode* y, int i, TypeNode* z)
{
	// New nodes for cleaning the RAM, executed by the garbage collector
	TypeNode* aux_x;
	TypeNode* aux_y;
	// Index for general purpose
	int j;


	// Creation of the new child node. This node will have the child nodes and the keys of the nodes y and z
	aux_y = new TypeNode();
	aux_y->n = y->n + 1 + z->n;
	aux_y->key = new int[aux_y->n];
	aux_y->c = new TypeNode*[aux_y->n + 1];
	aux_y->height = y->height;
	// Adding of the keys of the node y to the new child node at the beginning
	for (j = 0; j < y->n; j++)
		aux_y->key[j] = y->key[j];
	// Adding of the key i of the node x to the new child node at the middle
	aux_y->key[j] = x->key[i];
	// Adding of the key i of the node x to the new child node at the end
	for (j = 0; j < z->n; j++)
		aux_y->key[y->n + 1 + j] = z->key[j];
	// If the child nodes are not leafs
	if (y->height > 0)
	{
		// Adding of the child nodes of the node y at the beginning of the new child node
		for (j = 0; j < y->n + 1; j++)
			aux_y->c[j] = y->c[j];
		// Adding of the child nodes of the node z at the end of the new child node
		for (j = 0; j < z->n + 1; j++)
			aux_y->c[y->n + 1 + j] = z->c[j];
	}

	// Creation of the new node x that have the new child node as the child node at the i position
	aux_x = new TypeNode();
	// Setting of the height : equal to node x
	aux_x->height = x->height;
	// Of course the new node x has the number of child nodes as the node x decreased by one
	aux_x->c = new TypeNode*[x->n];
	// Adding of the child nodes
	for (j = 0; j < i; j++)
		aux_x->c[j] = x->c[j];
	// Adding of the new child node at the position i
	aux_x->c[i] = aux_y;
	// Completing of the child nodes
	for (j = i + 1; j < x->n; j++)
		aux_x->c[j] = x->c[j + 1];
	// Adding of the keys
	aux_x->key = new int[x->n - 1];
	for (j = 0; j < i; j++)
		aux_x->key[j] = x->key[j];
	for (j = i; j < x->n - 1; j++)
		aux_x->key[j] = x->key[j + 1];
	// Setting of the number of keys
	aux_x->n = x->n - 1;

	// Cleaning of the RAM (destroying the old nodes)
	delete(y);
	delete(z);
	delete(x);
	// End of function (the result is the new node x)
	return aux_x;
}


// This function balances the child nodes of the node x at the position i and i + 1 moving a number of keys and child nodes equal to KeysNumber : if KeysNumber is greater than 0 the objects are moved from right to left, otherwise they are moved from left to right. The key of the node x at the position i is set to the key of the right node at the position "KeysNumber" in the first case, or to the key of the left node at the position "KeysNumber" keys before the end of node
void CBTreesDlg::BTreeBalanceChilds(TypeNode* x, TypeNode* y, int i, TypeNode* z, int KeysNumber)
{
	// New nodes for cleaning the RAM, executed by the garbage collector
	TypeNode* aux_y;
	TypeNode* aux_z;
	// Index for general purpose
	int j;


	// Creation of the new child node of the node x at the position i
	aux_y = new TypeNode();
	// If KeysNumber is greater than 0 the number of keys is increased, otherwise it is decreased
	aux_y->n = y->n + KeysNumber;
	aux_y->key = new int[aux_y->n];
	aux_y->c = new TypeNode*[aux_y->n + 1];
	aux_y->height = y->height;
	// Creation of the new child node of the node x at the position i + 1
	aux_z = new TypeNode();
	// If KeysNumber is greater than 0 the number of keys is decreased, otherwise it is increased
	aux_z->n = z->n - KeysNumber;
	aux_z->key = new int[aux_z->n];
	aux_z->c = new TypeNode*[aux_z->n + 1];
	aux_z->height = z->height;
	// If KeysNumber is greater than 0 then the moving is from right to left
	if (KeysNumber > 0)
	{
		// Setting of the keys of the left node
		for (j = 0; j < y->n; j++)
			aux_y->key[j] = y->key[j];
		aux_y->key[j] = x->key[i];
		for (j = 0; j < KeysNumber - 1; j++)
			aux_y->key[y->n + 1 + j] = z->key[j];
		// Setting of the key of the node x at the position i
		x->key[i] = z->key[j];
		// Setting of the keys of the right node
		for (j = 0; j < z->n - KeysNumber; j++)
			aux_z->key[j] = z->key[KeysNumber + j];
		// If  the child nodes are not leafs then their child nodes must be set
		if (y->height > 0)
		{
			// Setting of the child nodes of the left node
			for (j = 0; j < y->n + 1; j++)
				aux_y->c[j] = y->c[j];
			for (j = 0; j < KeysNumber; j++)
				aux_y->c[y->n + 1 + j] = z->c[j];
			// Setting of the child nodes of the right node
			for (j = 0; j < z->n + 1 - KeysNumber; j++)
				aux_z->c[j] = z->c[KeysNumber + j];
		}
	}
	// If KeysNumber is smaller than 0 or equal to 0 then the moving is from left to right
	else
	{
		// It is necessary that the absolute value of KeysNumber is used
		KeysNumber = -KeysNumber;

		// Setting of the keys of the right node
		for (j = 0; j < z->n; j++)
			aux_z->key[KeysNumber + j] = z->key[j];
		aux_z->key[KeysNumber - 1] = x->key[i];
		for (j = 0; j < KeysNumber - 1; j++)
			aux_z->key[j] = y->key[y->n - KeysNumber + 1 + j];
		// Setting of the key of the node x at the position i
		x->key[i] = y->key[y->n - KeysNumber];
		// Setting of the keys of the left node
		for (j = 0; j < y->n - KeysNumber; j++)
			aux_y->key[j] = y->key[j];
		// If  the child nodes are not leafs then their child nodes must be set
		if (y->height > 0)
		{
			// Setting of the child nodes of the right node
			for (j = 0; j < z->n + 1; j++)
				aux_z->c[KeysNumber + j] = z->c[j];
			for (j = 0; j < KeysNumber; j++)
				aux_z->c[j] = y->c[y->n + 1 - KeysNumber + j];
			// Setting of the child nodes of the left node
			for (j = 0; j < y->n + 1 - KeysNumber; j++)
				aux_y->c[j] = y->c[j];
		}
	}

	// Changing of the child nodes of the node x
	x->c[i] = aux_y;
	x->c[i + 1] = aux_z;

	// Cleaning of the RAM (destroying the old child nodes)
	delete(y);
	delete(z);
}


// This function is a step of the processing of the B tree. A scan of the B tree starts from the leaf nodes. It takes 2 or 3 parameters : the node that must be processed, the new degree and optionally a boolean variable that is true if this is the first call of the function in the scan. It returns a pointer to the node that was processed
TypeNode* CBTreesDlg::BTreeChangeDegreeStep(TypeNode* r, int NewBTreeDegreeNumber, bool begin)
{
	// Dummy node for increasing the height of B tree
	TypeNode* s;
	// Pointers to child nodes
	TypeNode* y;
	TypeNode* z;
	// Index for general purpose
	int i;


	// If the new degree is smaller than the old one then the nodes of the tree will be splitted and, if it is necessary, the height of the tree will increase
	if (NewBTreeDegreeNumber < BTreeDegreeNumber)
	{
		// If the node is not a leaf
		if (r->height > 0)
		{
			// Visit of all subtrees of the processed node
			for (i = 0; i < r->n + 1; i++)
			{
				// The child node may be modified in the calls to the function at levels lower than this one
				y = BTreeChangeDegreeStep(r->c[i], NewBTreeDegreeNumber, false);
				r->c[i] = y;
			}

			// Scan of all child nodes of the processed node. This is while the processed node has not the maximum number of keys (the number of keys of every node must be equal or less than the maximum possible number of keys)
			i = 0;
			while (i < r->n + 1 && r->n < BTreeDegreeNumber * 2)
			{
				y = r->c[i];
				// If the number of keys is too great for the new degree, then the child node is splitted in two child nodes-> In this case the next node processed will be the first child node, otherwise the index is increased by one
				if (y->n > NewBTreeDegreeNumber * 2 - 1)
					r = BTreeSplitChild(r, i, y);
				else
					i++;
			}
		}
		// If the number of keys is too great and the processed node is the root, then the height of the B tree is increased by one : the processed node is splitted in two child nodes, the child nodes of a new created node that will be the new root of the B tree
		if (r->n > NewBTreeDegreeNumber * 2 - 1 && begin)
		{
			s = new TypeNode();
			s->height = r->height + 1;
			s->n = 0;
			s->c = new TypeNode*[1];
			s->c[0] = r;
			r = BTreeSplitChild(s, 0, r);
		}

		// If the node is not a leaf
		if (r->height > 0)
			// Scan of all child nodes of the processed node : if not less of one of them has a too great number of keys, then other steps of the processing of the B tree are necessary
			for (i = 0; i <= r->n; i++)
			{
				y = r->c[i];
				if (y->n > NewBTreeDegreeNumber * 2 - 1)
					BTreeChangeDegreeEnd = false;
			}
	}

	// If the new degree is greater than the old one then the nodes of the tree will be merged together and, if it is necessary, the height of the tree will decrease (the entire routine is executed if the node is not a leaf)
	if (NewBTreeDegreeNumber > BTreeDegreeNumber && r->height > 0)
	{
		// Visit of all subtrees of the processed node
		for (i = 0; i < r->n + 1; i++)
		{
			y = BTreeChangeDegreeStep(r->c[i], NewBTreeDegreeNumber, false);
			r->c[i] = y;
		}

		// Scan of all child nodes of the processed node
		i = 0;
		while (i < r->n)
		{
			y = r->c[i];
			z = r->c[i + 1];
			// If the sum of the numbers of keys is smaller than or equal to the maximum number of keys in the new degree, then the child nodes at the positions i and i + 1 can be merged together
			if (y->n + 1 + z->n <= NewBTreeDegreeNumber * 2 - 1)
			{
				// Only if the number of keys of the processed node is greater than 1 the two child nodes can be merged together (the new node must have more a key, the case of root is managed after). Otherwise nothing is done and the index is increased by one
				if (r->n > 1)
					r = BTreeMergeChilds(r, y, i, z);
				else
					i++;
			}
			// If the two child nodes can not be merged together
			else
			{
				// If one of the two child nodes has the number of keys too small for the new degree (the case of both child nodes is impossible because in this case the sum is smaller than the maximum number of keys), then the child nodes are balanced with the movement of some keys from one to the other
				if (y->n < NewBTreeDegreeNumber - 1 || z->n < NewBTreeDegreeNumber - 1)
					BTreeBalanceChilds(r, y, i, z, NewBTreeDegreeNumber - 1 - y->n);

				// The index is increased by one
				i++;
			}
		}
		// If the number of keys is 1 and the processed node is the root, then the two child nodes are merged together, the new node becomes the new root and the old root is destroyed (the height of the B tree is decreased by one)
		if (r->n == 1 && begin)
		{
			y = r->c[0];
			z = r->c[1];
			if (y->n + 1 + z->n <= NewBTreeDegreeNumber * 2 - 1)
			{
				s = BTreeMergeChilds(r, y, 0, z);
				r = s->c[0];
				delete(s);
			}
		}

		// If the node is not a leaf
		if (r->height > 0)
			// Scan of all child nodes of the processed node : if not less of one of them has a too small number of keys, then other steps of the processing of the B tree are necessary
			for (i = 0; i <= r->n; i++)
			{
				y = r->c[i];
				if (y->n < NewBTreeDegreeNumber - 1)
					BTreeChangeDegreeEnd = false;
			}
	}

	// End of function (the pointer to the processed node is returned because it may have been modified respect to the beginning)
	return r;
}


// The main function : it contains the main loop of steps for processing of the B tree. It ends when all nodes of the B tree have the number of keys between the minimum number of keys and the maximum number of keys for the new degree. It returns the pointer to the root of the new B tree
TypeNode* CBTreesDlg::BTreeChangeDegree(TypeNode* r, int NewBTreeDegreeNumber)
{
	do
	{
		// The flag of the termination of the loop is set
		BTreeChangeDegreeEnd = true;
		// Process of the B tree
		r = BTreeChangeDegreeStep(r, NewBTreeDegreeNumber);
		// If the flag of the termination has not been modified by the process, then the loop terminates
	} while (!BTreeChangeDegreeEnd);

	// End of function (the pointer to the new B tree is returned)
	return r;
}


void CBTreesDlg::OnBnClickedLoadbutton()
{
	CString BTreeDegreeText;
	char BTreeDegreeBuffer[10];
	CFileDialog DataFileDialog(TRUE);
	char *DataBuffer;
	char *DataString;
	int Data;
	char *NextData;
	CFile DataFile;
	int DataFileLen;


	if (BTreeDegree.GetWindowTextLengthW() == 0)
	{
		CString message("Insert the value of the degree.");
		CString caption("B trees");

		MessageBox(message, caption, MB_ICONERROR);
		return;
	}
	BTreeDegree.GetWindowTextW(BTreeDegreeText);
	for (int i = 0; i < BTreeDegreeText.GetLength() && i < 10; i++)
		BTreeDegreeBuffer[i] = (char)BTreeDegreeText[i];
	if (BTreeDegreeText.GetLength() < 10)
		BTreeDegreeBuffer[BTreeDegreeText.GetLength()] = '\0';
	BTreeDegreeNumber = atoi(BTreeDegreeBuffer);
	if (BTreeDegreeNumber < 2)
	{
		CString message("Wrong value of the degree!");
		CString caption("B trees");

		MessageBox(message, caption, MB_ICONERROR);
		return;
	}

	//	for (int i=999;i>=0;i--)
	//		BTreeInsert(BTree, i);
	DataFileDialog.DoModal();
	if (DataFileDialog.GetFileName().IsEmpty())
		return;
	BTreeElimination(BTree);
	BTree = NULL;

	if (!SetCurrentDirectory(DataFileDialog.GetFolderPath()))
	{
		CString message("Impossible change of the work directory");
		CString caption("B trees");

		MessageBox(message, caption, MB_ICONEXCLAMATION);
		return;
	}
	if (!DataFile.Open(DataFileDialog.GetFileName(), CFile::modeRead))
	{
		CString message("Impossible open of the file!");
		CString caption("B trees");

		MessageBox(message, caption, MB_ICONEXCLAMATION);
		return;
	}
	DataFileLen = (int)DataFile.GetLength();
	DataBuffer = new char[DataFileLen];
	if (DataFile.Read(DataBuffer, DataFileLen) == 0)
	{
		CString message("Error during the read of the file!");
		CString caption("B trees");

		MessageBox(message, caption, MB_ICONSTOP);
		return;
	}
	DataString = strtok_s(DataBuffer, "\r\n", &NextData);
	while (DataString != NULL)
	{
		Data = atoi(DataString);
		BTreeInsert(BTree, Data);
		DataString = strtok_s(NULL, "\r\n", &NextData);
	}
	DataFile.Close();

	BTreeVisualization(BTree);

	delete(DataBuffer);
}


void CBTreesDlg::OnBnClickedChangedegreebutton()
{
	CString BTreeDegreeText;
	char BTreeDegreeBuffer[5];
	int NewBTreeDegreeNumber;
	TypeNode* r;


	if (BTree == NULL)
	{
		CString message("You must load a tree before.");
		CString caption("B trees");

		MessageBox(message, caption, MB_ICONERROR);
		return;
	}
	if (BTreeDegree.GetWindowTextLengthW() == 0)
	{
		CString message("Insert the value of the degree.");
		CString caption("B trees");

		MessageBox(message, caption, MB_ICONERROR);
		return;
	}
	BTreeDegree.GetWindowTextW(BTreeDegreeText);
	for (int i = 0; i < BTreeDegreeText.GetLength() && i < 10; i++)
		BTreeDegreeBuffer[i] = (char)BTreeDegreeText[i];
	if (BTreeDegreeText.GetLength() < 10)
		BTreeDegreeBuffer[BTreeDegreeText.GetLength()] = '\0';
	NewBTreeDegreeNumber = atoi(BTreeDegreeBuffer);
	if (NewBTreeDegreeNumber < 2)
	{
		CString message("Wrong value of the degree!");
		CString caption("B trees");

		MessageBox(message, caption, MB_ICONERROR);
		return;
	}
	r = BTree;
	BTree = BTreeChangeDegree(r, NewBTreeDegreeNumber);
	BTreeDegreeNumber = NewBTreeDegreeNumber;
	BTreeVisualization(BTree);
	CString message("Degree changed.");
	CString caption("B trees");
	MessageBox(message, caption, MB_ICONINFORMATION);
}
