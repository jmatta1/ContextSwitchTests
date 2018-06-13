.PHONY: all
all: testThread testFiber

.PHONY: testThreads
testThread:
	$(MAKE) -C TestThreads release
	@cp ./TestThreads/testThreads ./testThread

.PHONY: testFibers
testFiber:
	$(MAKE) -C TestFibers release
	@cp ./TestFibers/testFibers ./testFiber

.PHONY: clean
clean:
	-$(MAKE) -C TestThreads superclean
	-$(MAKE) -C TestFibers superclean
	-rm -f ./testFiber
	-rm -f ./testThread
