#!/usr/bin/python

import unittest, judy

from test.mapping_tests import TestMappingProtocol

class JudyTestMappingProtocol(TestMappingProtocol):
	type2test = judy.JudyIntObjectMap

	def test_update(self):
		raise unittest.SkipTest('update not supported yet')

del TestMappingProtocol
