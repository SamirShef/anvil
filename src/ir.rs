pub type InstId = usize;
pub type BasicBlockId = usize;
pub type FunctionId = usize;

pub enum Inst {
    Binary(InstId, InstId),
    Call(FunctionId),
    Jump(BasicBlockId),
    JumpIf(InstId, BasicBlockId, BasicBlockId),
    Alloca,
    Load(InstId),
    Store(InstId),
}

pub struct BasicBlock {
    pub body: Vec<Inst>,
}

pub struct Function {
    pub body: Vec<BasicBlock>,
}
