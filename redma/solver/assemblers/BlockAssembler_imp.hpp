namespace RedMA
{

template <class InVectorType, class InMatrixType>
BlockAssembler<InVectorType, InMatrixType>::
BlockAssembler(const GetPot& datafile, const TreeStructure& tree) :
  aAssembler<InVectorType, InMatrixType>(datafile),
  M_tree(tree)
{

}

template <class InVectorType, class InMatrixType>
void
BlockAssembler<InVectorType, InMatrixType>::
setup()
{
    typedef std::map<unsigned int, SHP(TreeNode)>         NodesMap;
    typedef aAssembler<VInner, MInner>                    InnerAssembler;
    typedef std::vector<SHP(TreeNode)>                    NodesVector;

    NodesMap nodesMap = M_tree.getNodesMap();

    // allocate assemblers
    for (NodesMap::iterator it = nodesMap.begin(); it != nodesMap.end(); it++)
    {
        SHP(InnerAssembler) newAssembler;
        newAssembler = AssemblerFactory<VInner, MInner> (M_datafile, it->second);
        newAssembler->setup();
        M_primalAssemblers[it->second->M_ID] = newAssembler;
    }

    // allocate interface assemblers
    for (NodesMap::iterator it = nodesMap.begin(); it != nodesMap.end(); it++)
    {
        NodesVector children = it->second->M_children;

        unsigned int countOutlet = 0;
        unsigned int myID = it->second->M_ID;

        SHP(InnerAssembler) fatherAssembler = M_primalAssemblers[myID];
        for (NodesVector::iterator itVector = children.begin();
             itVector != children.end(); itVector++)
        {
            if (*itVector)
            {
                unsigned int otherID = (*itVector)->M_ID;
                SHP(InnerAssembler) childAssembler = M_primalAssemblers[otherID];
                Interface<VInner, MInner> newInterface(fatherAssembler, myID,
                                                       childAssembler, otherID);
            }
        }
    }
    // unsigned int count = 0;
    // for (auto it = M_dimensionsVector.begin(); it != M_dimensionsVector.end(); it++)
    //     count += *it;
    // std::string msg = "[GlobalAssembler] number of primal + dual dofs = ";
    // msg += std::to_string(count);
    // msg += "\n";
    // printlog(MAGENTA, msg, M_verbose);
}


template <class InVectorType, class InMatrixType>
void
BlockAssembler<InVectorType, InMatrixType>::
exportSolution(const double& t)
{

}

template <class InVectorType, class InMatrixType>
void
BlockAssembler<InVectorType, InMatrixType>::
postProcess()
{

}

template <class InVectorType, class InMatrixType>
BlockMatrix<InMatrixType>
BlockAssembler<InVectorType, InMatrixType>::
getMass(const double& time, const BlockVector<InVectorType>& sol)
{

}

template <class InVectorType, class InMatrixType>
BlockVector<InVectorType>
BlockAssembler<InVectorType, InMatrixType>::
getRightHandSide(const double& time, const BlockVector<InVectorType>& sol)
{

}

template <class InVectorType, class InMatrixType>
BlockMatrix<InMatrixType>
BlockAssembler<InVectorType, InMatrixType>::
getJacobianRightHandSide(const double& time, const BlockVector<InVectorType>& sol)
{

}

}
