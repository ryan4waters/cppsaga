int main(int argc, char* argv[])
{
	[](this auto f)->void { f(); }();
	return 0;
}