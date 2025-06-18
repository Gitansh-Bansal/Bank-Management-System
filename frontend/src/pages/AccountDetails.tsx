import React, { useState, useEffect } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import {
  Container,
  Box,
  Typography,
  Button,
  Paper,
  TextField,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
} from '@mui/material';
import axios from 'axios';

interface Transaction {
  timestamp: string;
  type: string;
  amount: number;
  relatedAccount?: string;
  balance: number;
}

interface Account {
  accountNumber: number;
  type: string;
  balance: number;
}

export default function AccountDetails() {
  const { accountNumber } = useParams<{ accountNumber: string }>();
  const [account, setAccount] = useState<Account | null>(null);
  const [transactions, setTransactions] = useState<Transaction[]>([]);
  const [openDialog, setOpenDialog] = useState(false);
  const [transactionType, setTransactionType] = useState('');
  const [amount, setAmount] = useState('');
  const [targetAccount, setTargetAccount] = useState('');
  const [password, setPassword] = useState('');
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [transactionLoading, setTransactionLoading] = useState(false);
  const [transactionError, setTransactionError] = useState<string | null>(null);
  const [closeDialogOpen, setCloseDialogOpen] = useState(false);
  const [closePassword, setClosePassword] = useState('');
  const [closeLoading, setCloseLoading] = useState(false);
  const [closeError, setCloseError] = useState<string | null>(null);
  const navigate = useNavigate();

  useEffect(() => {
    if (accountNumber) {
      fetchAccountDetails();
      fetchTransactions();
    }
  }, [accountNumber]);

  const fetchAccountDetails = async () => {
    try {
      setLoading(true);
      setError(null);
      const response = await axios.get(`http://localhost:3001/api/accounts/${accountNumber}`);
      setAccount(response.data);
    } catch (error) {
      console.error('Failed to fetch account details:', error);
      setError('Failed to load account details');
    } finally {
      setLoading(false);
    }
  };

  const fetchTransactions = async () => {
    try {
      const response = await axios.get(`http://localhost:3001/api/transactions/${accountNumber}`);
      setTransactions(response.data.transactions || []);
    } catch (error) {
      console.error('Failed to fetch transactions:', error);
      setTransactions([]);
    }
  };

  const handleTransaction = async () => {
    try {
      console.log('Starting transaction:', { transactionType, accountNumber, amount });
      
      // Validate amount
      const amountValue = parseFloat(amount);
      if (isNaN(amountValue) || amountValue <= 0) {
        setTransactionError('Please enter a valid positive amount');
        return;
      }
      
      setTransactionLoading(true);
      setTransactionError(null);
      
      const endpoint = transactionType === 'transfer'
        ? '/api/transactions/transfer'
        : `/api/transactions/${transactionType}`;

      const data = transactionType === 'transfer'
        ? {
            fromAccount: accountNumber,
            toAccount: targetAccount,
            amount: amountValue,
            password,
          }
        : {
            accountNumber: accountNumber?.toString(),
            amount: amountValue,
            password,
          };

      console.log('Making request to:', `http://localhost:3001${endpoint}`);
      console.log('Request data (without password):', { 
        ...data, 
        password: '[HIDDEN]' 
      });

      const response = await axios.post(`http://localhost:3001${endpoint}`, data);
      console.log('Transaction response:', response.data);
      
      setOpenDialog(false);
      resetDialogState();
      fetchAccountDetails();
      fetchTransactions();
    } catch (error: any) {
      console.error('Transaction failed:', error);
      console.error('Error response:', error.response);
      setTransactionError(error.response?.data?.error || 'Transaction failed');
    } finally {
      setTransactionLoading(false);
    }
  };

  const resetDialogState = () => {
    setAmount('');
    setTargetAccount('');
    setPassword('');
    setTransactionType('');
    setTransactionError(null);
    setTransactionLoading(false);
  };

  const handleCloseDialog = () => {
    setOpenDialog(false);
    resetDialogState();
  };

  const handleCloseAccount = async () => {
    try {
      setCloseLoading(true);
      setCloseError(null);
      
      await axios.delete(`http://localhost:3001/api/accounts/${accountNumber}`, {
        data: { password: closePassword }
      });
      
      setCloseDialogOpen(false);
      setClosePassword('');
      navigate('/dashboard');
    } catch (error: any) {
      console.error('Failed to close account:', error);
      setCloseError(error.response?.data?.error || 'Failed to close account');
    } finally {
      setCloseLoading(false);
    }
  };

  return (
    <Container>
      <Box sx={{ mt: 4, mb: 4 }}>
        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 4 }}>
          <Typography variant="h4" component="h1">
            Account Details
          </Typography>
          <Button variant="outlined" color="primary" onClick={() => navigate('/dashboard')}>
            Back to Dashboard
          </Button>
        </Box>

        {loading && (
          <Paper sx={{ p: 3, mb: 4, textAlign: 'center' }}>
            <Typography>Loading account details...</Typography>
          </Paper>
        )}

        {error && (
          <Paper sx={{ p: 3, mb: 4, bgcolor: 'error.light', color: 'error.contrastText' }}>
            <Typography>{error}</Typography>
          </Paper>
        )}

        {account && !loading && (
          <Paper sx={{ p: 3, mb: 4 }}>
            <Typography variant="h6">Account #{account.accountNumber}</Typography>
            <Typography color="text.secondary">Type: {account.type}</Typography>
            <Typography variant="h5" sx={{ mt: 2 }}>
              Balance: ${account.balance.toFixed(2)}
            </Typography>
          </Paper>
        )}

        {account && !loading && (
          <Box sx={{ mb: 4 }}>
            <Button
              variant="contained"
              color="primary"
              onClick={() => {
                setTransactionType('deposit');
                setOpenDialog(true);
              }}
              sx={{ mr: 2 }}
            >
              Deposit
            </Button>
            <Button
              variant="contained"
              color="primary"
              onClick={() => {
                setTransactionType('withdraw');
                setOpenDialog(true);
              }}
              sx={{ mr: 2 }}
            >
              Withdraw
            </Button>
            <Button
              variant="contained"
              color="primary"
              onClick={() => {
                setTransactionType('transfer');
                setOpenDialog(true);
              }}
              sx={{ mr: 2 }}
            >
              Transfer
            </Button>
            <Button
              variant="contained"
              color="error"
              onClick={() => setCloseDialogOpen(true)}
            >
              Close Account
            </Button>
          </Box>
        )}

        {account && !loading && (
          <>
            <Typography variant="h5" sx={{ mb: 2 }}>
              Transaction History
            </Typography>
            <TableContainer component={Paper}>
              <Table>
                <TableHead>
                  <TableRow>
                    <TableCell>Date & Time</TableCell>
                    <TableCell>Type</TableCell>
                    <TableCell>Amount</TableCell>
                    <TableCell>Related Account</TableCell>
                    <TableCell>Balance</TableCell>
                  </TableRow>
                </TableHead>
                <TableBody>
                  {transactions.length === 0 ? (
                    <TableRow>
                      <TableCell colSpan={5} align="center">
                        No transactions found
                      </TableCell>
                    </TableRow>
                  ) : (
                    transactions.map((transaction, index) => (
                      <TableRow key={index}>
                        <TableCell>{transaction.timestamp}</TableCell>
                        <TableCell>{transaction.type}</TableCell>
                        <TableCell>${Math.abs(transaction.amount).toFixed(2)}</TableCell>
                        <TableCell>{transaction.relatedAccount || '-'}</TableCell>
                        <TableCell>${transaction.balance.toFixed(2)}</TableCell>
                      </TableRow>
                    ))
                  )}
                </TableBody>
              </Table>
            </TableContainer>
          </>
        )}

        <Dialog open={openDialog} onClose={handleCloseDialog}>
          <DialogTitle>
            {transactionType === 'deposit'
              ? 'Deposit'
              : transactionType === 'withdraw'
              ? 'Withdraw'
              : 'Transfer'}
          </DialogTitle>
          <DialogContent>
            {transactionError && (
              <Typography color="error" sx={{ mb: 2 }}>
                {transactionError}
              </Typography>
            )}
            {transactionType === 'transfer' && (
              <TextField
                fullWidth
                label="Target Account Number"
                value={targetAccount}
                onChange={(e) => setTargetAccount(e.target.value)}
                margin="normal"
              />
            )}
            <TextField
              fullWidth
              label="Amount"
              type="number"
              value={amount}
              onChange={(e) => setAmount(e.target.value)}
              margin="normal"
            />
            <TextField
              fullWidth
              label="Account Password"
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
              margin="normal"
            />
          </DialogContent>
          <DialogActions>
            <Button onClick={handleCloseDialog} disabled={transactionLoading}>
              Cancel
            </Button>
            <Button 
              onClick={handleTransaction} 
              variant="contained" 
              color="primary"
              disabled={transactionLoading}
            >
              {transactionLoading 
                ? 'Processing...' 
                : transactionType === 'deposit'
                ? 'Deposit'
                : transactionType === 'withdraw'
                ? 'Withdraw'
                : 'Transfer'
              }
            </Button>
          </DialogActions>
        </Dialog>

        <Dialog open={closeDialogOpen} onClose={() => setCloseDialogOpen(false)}>
          <DialogTitle>Close Account</DialogTitle>
          <DialogContent>
            <Typography sx={{ mb: 2 }}>
              Are you sure you want to close account #{accountNumber}? This action cannot be undone.
            </Typography>
            {closeError && (
              <Typography color="error" sx={{ mb: 2 }}>
                {closeError}
              </Typography>
            )}
            <TextField
              fullWidth
              label="Account Password"
              type="password"
              value={closePassword}
              onChange={(e) => setClosePassword(e.target.value)}
              margin="normal"
            />
          </DialogContent>
          <DialogActions>
            <Button onClick={() => setCloseDialogOpen(false)} disabled={closeLoading}>
              Cancel
            </Button>
            <Button 
              onClick={handleCloseAccount} 
              variant="contained" 
              color="error"
              disabled={closeLoading}
            >
              {closeLoading ? 'Closing...' : 'Close Account'}
            </Button>
          </DialogActions>
        </Dialog>
      </Box>
    </Container>
  );
} 